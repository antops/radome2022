import vtkmodules.all as vtk
from vtkmodules.util.numpy_support import vtk_to_numpy, numpy_to_vtk
import matplotlib.pyplot as plt
import time
import sys
import numpy as np
import time
import os
import rayInitial
import raytracingRT
from initial_input import initData
from initial_input import initSource
from non_radome import calculate_nonFarField_new
sys.path.append(".")  # 把上级目录加入模块搜索
from GaussianFuncs import AdaptiveGBFuncsV3 as AGB #自定义高斯分解合成相关模块


if getattr(sys, 'frozen', False):
    path = os.path.dirname(sys.executable)
elif __file__:
    path = os.path.dirname(__file__)

#清空目录
def del_file(path):
    ls = os.listdir(path)
    for i in ls:
        c_path = os.path.join(path, i)
        if os.path.isdir(c_path):#如果是文件夹那么递归调用一下
            del_file(c_path)
        else:                    #如果是一个文件那么直接删除
            os.remove(c_path)
    # print ('文件已经清空完成')

#调用初始化接口文件
version = 1
# print(sys.argv)
jsopath = sys.argv[1]
# jsopath='meta.json'
syspath=os.path.dirname(os.path.realpath(sys.argv[0]))
content = initData(jsopath)
if content["version"] != version :
    print('version error',flush=True)
    sys.exit()

#建立坐标系
RefOrigin=np.array([0., 0, 0.])
RefAxisN=np.array([0., 0, 1.])
RefAxisU=np.array([1., 0, 0.])
RefAxisV=np.array([0., 1, 0.])
OutOrigin=np.array([0., 0, 0.])
#加载入射场
sourcepath=content['source_path']
polarization_mode=content['source_mode']
# sourcepath='E:/GB-VTK-Radome-RT-2.0/output/Efield17angle5.txt'
# print(sourcepath,flush=True)
FieldInput=initSource(sourcepath,polarization_mode)
if polarization_mode == 1:  #激励源水平/垂直极化选择
    FieldInput=FieldInput[:,:,1] #垂直极化，Ey有值
else:
    FieldInput=FieldInput[:,:,0]  #水平极化，Ex有值
# FieldInput=FieldInput[:,:,1]
# maxinput=max(np.abs(FieldInput))
np.save(syspath + "\\1SourceField\\FieldInput.npy",FieldInput)

#仿真参数
threads_per_block = content['threads']
c0 = 2.99792e8
f0 = content['f0']
ds = content['ds']
lamda = c0 / f0
print('波长= ',lamda)
k0 = 2 * np.pi / lamda
dx, dy, dz = ds, ds, 0.1
RTdepth=6
numofLayer=len(content['stl_meta'])
# 媒质参数
mu=np.zeros(numofLayer+1,dtype=complex)
eps=np.zeros(numofLayer+1,dtype=complex)
# lossTan=np.zeros(numofLayer+1)
mu[0]=content['background_material']['mu0']
eps[0]=content['background_material']['eps0']
for ll in range(numofLayer-1):
    lossTan=content['model_meta'][ll]['material']['lossT']
    mu[ll+1]=content['model_meta'][ll]['material']['mu0']
    eps[ll+1]=content['model_meta'][ll]['material']['eps0'] * (1 - 1j * lossTan)
mu[numofLayer]=content['background_material']['mu0']
eps[numofLayer]=content['background_material']['eps0']

#远场角度
maxtheta=content['farfield']['maxtheta']
numtheta=content['farfield']['numtheta']
maxphi=content['farfield']['maxphi']
numphi=content['farfield']['numphi']
angle_phi=0 #波束指向方位角
theta = np.linspace(0, maxtheta, num=numtheta)
theta = theta * np.pi/180.0
phi = np.linspace(0, maxphi, num=numphi)
phi = phi * np.pi / 180.0
#入射场网格化，画图
(Xrange,Yrange)=FieldInput.shape
print('激励源阵列点数', Xrange,Yrange,flush=True)
Xmin,Xmax=-(Xrange-1)/2*dx,(Xrange-1)/2*dx
Ymin,Ymax=-(Yrange-1)/2*dy,(Yrange-1)/2*dy
XmeshIn,YmeshIn=np.mgrid[Xmin:Xmax:Xrange*1j,Ymin:Ymax:Yrange*1j]
ZmeshIn=np.zeros(XmeshIn.shape)
# plt.contourf(np.abs(FieldInput), 50, cmap='rainbow') # X为列数， Y为行数
# plt.show()

del_file(syspath + "\\3layerInfo")   #清空模型信息文件夹
temp=[] #临时存储各层信息准备合并
tempmedia=[] #临时存储各层媒质参数准备合并

for n in range(numofLayer):
    # stlpath='4STL/pingban_'+str(n)+'.stl'
    stlpath=content['stl_meta'][n]['stl_path']
    readerSTL=vtk.vtkSTLReader()
    readerSTL.SetFileName(stlpath)
    GeomFilter=vtk.vtkTriangleFilter()
    GeomFilter.SetInputConnection(readerSTL.GetOutputPort())
    GeomFilter.Update()
    normalFilter=vtk.vtkPolyDataNormals() #处理法向量
    normalFilter.SetInputConnection(readerSTL.GetOutputPort())
    normalFilter.SetComputePointNormals(1)
    normalFilter.SetComputeCellNormals(1)
    normalFilter.SetAutoOrientNormals(1)
    normalFilter.Update()
    norm_tri = vtk_to_numpy(normalFilter.GetOutput().GetCellData().GetNormals())#获得各层面元法向量
    flipindex=np.argwhere(norm_tri[:,2]<0)
    if len(flipindex)>0.5*len(norm_tri[:,2]):
        norm_tri=-norm_tri
    numcls=len(norm_tri[:,0])
    rtpPolydata = vtk.vtkPolyData()
    rtpPolydata = GeomFilter.GetOutput()
    rtpVertices = vtk_to_numpy(rtpPolydata.GetPoints().GetData())  # 导出点位置
    rtpIndices = np.zeros([numcls, 9], dtype=np.float32)  # 导出点ID
    cellmedia = np.zeros([numcls, 4], dtype=complex)    #记录媒质参数
    layerID = np.ones([numcls,1], dtype=int)*(numofLayer-n-1)    #记录媒质参数,最外层编号为0，便于RT函数内判断出射
    cpIds = vtk.vtkIdList()
    print('第'+str(n)+'层面元数量: '+str(numcls),flush=True)
    for i in range(numcls):     #按（x1, y1, z1, x2, y2, z2, x3, y3, z3）组织面元顶点坐标
        rtpPolydata.GetCellPoints(i, cpIds)
        for j in range(cpIds.GetNumberOfIds()):
            rtpIndices[i][j * 3] = rtpVertices[cpIds.GetId(j)][0]
            rtpIndices[i][j * 3 + 1] = rtpVertices[cpIds.GetId(j)][1]
            rtpIndices[i][j * 3 + 2] = rtpVertices[cpIds.GetId(j)][2]
    np.save((syspath + '\\3layerInfo\\cord_indi_' + str(n) + '.npy'), rtpIndices)
    # if n==numofLayer-1:
    #     np.save((".\\3layerInfo\\cord_indi_outer.npy"), rtpIndices) #记录最外层面元顶点坐标信息

    cellmedia[:,0]=mu[n]
    cellmedia[:,1]=eps[n]
    cellmedia[:,2]=mu[n+1]
    cellmedia[:,3]=eps[n+1]
    if n==0:
        polydata = vtk.vtkGeometryFilter()
        polydata.SetInputConnection(normalFilter.GetOutputPort())
        polydata.Update()
        pointsCordinates = vtk_to_numpy(polydata.GetOutput().GetPoints().GetData())
        norm_pts_inner = vtk_to_numpy(polydata.GetOutput().GetPointData().GetNormals())  # 获得最内层节点法向量
        norm_tri_inner = vtk_to_numpy(polydata.GetOutput().GetCellData().GetNormals())  # 获得最内层面元法向量
        flipindex1=np.argwhere(norm_pts_inner[:,2]<0)
        flipindex2=np.argwhere(norm_tri_inner[:,2]<0)
        if len(flipindex1)>0.5*len(norm_pts_inner[:,2]):
            norm_pts_inner=-norm_pts_inner
        if len(flipindex2)>0.5*len(norm_tri_inner[:,2]):
            norm_tri_inner=-norm_tri_inner
        np.save((syspath + "\\3layerInfo\\norm_pts_inner.npy"), norm_pts_inner)
        np.save((syspath + "\\3layerInfo\\norm_tri_inner.npy"), norm_tri_inner)
        np.save((syspath + "\\3layerInfo\\cord_pts_inner.npy"), pointsCordinates)
        np.save((syspath + "\\3layerInfo\\cord_indi_inner.npy"), rtpIndices)
    layerInfo=np.concatenate([norm_tri,rtpIndices,layerID],axis=1) #记录各层信息：法向量，面元3顶点坐标，该层编号
    np.save((syspath + "\\3layerInfo\\layer" + str(n) + ".npy"), layerInfo)
    temp.append(layerInfo)
    tempmedia.append(cellmedia)
allLayerInfo=temp[0]
allmedia=tempmedia[0]
for n in range(numofLayer-1):
    info1=allLayerInfo
    info2=temp[n+1]
    allLayerInfo=np.concatenate([info1,info2])
    media1=allmedia
    media2=tempmedia[n+1]
    allmedia=np.concatenate([media1,media2])
np.save((syspath + "\\3layerInfo\\allLayerInfo.npy"), allLayerInfo)####拼接所有层的信息
np.save((syspath + "\\3layerInfo\\allmedia.npy"), allmedia)####拼接所有层的媒质参数

#高斯分解入射场
AGB.AdaptiveDecomposition(np.load(syspath + "\\1SourceField\\FieldInput.npy"), syspath + "\\2AdaptiveGB\\GaussInfo.txt",dx,dy)# 输出 GaussInfo.txt

tmparea=[]
for aa in range(numofLayer):
    rtpIndices=np.load(syspath + '\\3layerInfo\\cord_indi_'+str(aa)+'.npy')
    vec1 = rtpIndices[:, :3] - rtpIndices[:, 3:6]
    vec2 = rtpIndices[:, 6:] - rtpIndices[:, 3:6]
    cellarea = np.linalg.norm(np.cross(vec1, vec2), axis=1) * 0.5
    if aa==0:
        tmptile = np.tile(np.array(1 / 3).transpose(), (1, 3))
        cellcenter = (rtpIndices[:, :3] + rtpIndices[:, 3:6] + rtpIndices[:, 6:]) * tmptile
        np.save((syspath + "\\3layerInfo\\cellcenter_inner.npy"), cellcenter)
        np.save((syspath + "\\3layerInfo\\cellarea_inner.npy"), cellarea)
    tmparea.append(cellarea)
cellarea_all=tmparea[0]
for n in range(numofLayer-1):
    info1=cellarea_all
    info2=tmparea[n+1]
    cellarea_all=np.concatenate([info1,info2])
np.save((syspath + "\\3layerInfo\\cellarea_all.npy"), cellarea_all)
cellcenter=np.load(syspath + "\\3layerInfo\\cellcenter_inner.npy")
cellarea_inner=np.load(syspath + "\\3layerInfo\\cellarea_inner.npy")

# 计算内表面被照射场分布
if polarization_mode == 1:
    CalcFlag = np.array([0, 1, 1, 1, 1, 1])
else:
    CalcFlag=np.array([1, 0, 1, 1, 1, 1])
FieldOnRadome=AGB.FieldOnSurface(k0,np.loadtxt(syspath + "\\2AdaptiveGB\\GaussInfo.txt"),cellcenter,syspath + "\\2AdaptiveGB\\FieldOnRadome.npy",syspath + "\\2AdaptiveGB\\PoyntingVectors.npy",CalcFlag) #输出全部电磁场分布文件

fdx,fdy = np.where(np.isnan(FieldOnRadome))
# print("Done",flush=True)

PoyntingVectors=np.load(syspath + "\\2AdaptiveGB\\PoyntingVectors.npy").astype(np.float32)
PoyntingVectorsZ=PoyntingVectors[:,2]
FieldExEyEz=np.load(syspath + "\\2AdaptiveGB\\FieldOnRadome.npy") [:,:3]          # (Ex, Ey, Ez)
absPV=np.sqrt(PoyntingVectors[:,0]*PoyntingVectors[:,0]+PoyntingVectors[:,1]*PoyntingVectors[:,1]+PoyntingVectors[:,2]*PoyntingVectors[:,2])
IndexPV=np.argwhere(absPV>0.99).ravel() # 过滤标准为1e-3

#初始化入射光线
Etile = np.tile(np.array(2).transpose(), (1, 3))
numofRay=len(IndexPV)
# incfield=FieldExEyEz[IndexPV]
incfield=FieldExEyEz[IndexPV]/Etile
incorig=cellcenter[IndexPV]
incx=incorig[:,0]
incy=incorig[:,1]
incz=np.abs(incfield[:,1])

# print('maxinc: ',maxinc,'maxinput: ',maxinput)

incarea=cellarea_inner[IndexPV]
PoyntingVectors_index=PoyntingVectors[IndexPV]
norm_tri_inner=norm_tri_inner[IndexPV]
norm_tri_inner_z=norm_tri_inner[:,2]
for i in range(numofRay):
    PoyntingVectors_index[i]=PoyntingVectors_index[i]/np.linalg.norm(PoyntingVectors_index[i])  #Poynting归一化
incvec=PoyntingVectors_index
incINDEX=np.zeros(numofRay,dtype=int)
missfield=np.zeros((0,3),dtype=complex)

vecabs = np.linalg.norm(incvec, axis=1)
IndexVec = np.argwhere(vecabs > 1e-7).ravel()
incorig = incorig[IndexVec]
incvec = incvec[IndexVec]
incfield = incfield[IndexVec]
incINDEX = incINDEX[IndexVec]
incarea = incarea[IndexVec]
norm_tri_inner=norm_tri_inner[IndexVec]

coecos_inner = np.sum(norm_tri_inner*incvec,axis=1)
cosindex=np.argwhere(coecos_inner<0)
incarea_cos = np.ones_like(incarea)#/coecos_inner###########*

del_file(syspath + "\\4Nearfield")
np.save(syspath + '\\4Nearfield\\incorig.npy',incorig)
np.save(syspath + '\\4Nearfield\\incfield.npy',incfield)
start=time.time()#光追计时
#光线追踪
rayInitial.rayInit(incfield,incorig,incvec,incINDEX,incarea,norm_tri_inner,allLayerInfo,allmedia,k0,RTdepth,numofLayer,numofRay, threads_per_block)

end=time.time()
savecnt=raytracingRT.cnt+1  #记录保存了多少组文件
raytracingRT.cnt=-1#############全局变量更新，便于后续批量扫参
print('光追计时: ',end-start,'s','保存次数: ',savecnt,flush=True)

#远场计算
eps0 = 8.854e-12
mu0 = 4 * np.pi * 1e-7
c = 1 / np.sqrt(eps0 * mu0)
Z0 = np.sqrt(mu0 / eps0)
[theaxx,phiyy] = np.meshgrid(theta,phi)
dtheta = maxtheta/(numtheta-1)
dphi = maxphi/(numphi-1)
xxp=np.cos(phiyy)*np.sin(theaxx)
yyp=np.sin(phiyy)*np.sin(theaxx)

tmporig=[]
tmpvec=[]
tmpE=[]
tmpN=[]
tmptri=[]

for nn in range(savecnt):
    tmporig.append(np.load(syspath + '\\4Nearfield\\missorig' + str(nn) + '.npy'))
    tmpvec.append(np.load(syspath + '\\4Nearfield\\missvec' + str(nn) + '.npy'))
    tmpE.append(np.load(syspath + '\\4Nearfield\\missfield' + str(nn) + '.npy'))
    tmpN.append(np.load(syspath + '\\4Nearfield\\misscellN' + str(nn) + '.npy'))
    tmptri.append(np.load(syspath + '\\4Nearfield\\misscellINDEX' + str(nn) + '.npy').reshape(-1,1))

r_cell = tmporig[0]
i_poynting = tmpvec[0]
E_XYZ=tmpE[0]
normal=tmpN[0]
triINDEX=tmptri[0]
if savecnt > 0:
    for nn in range(savecnt-1):
        r_cell1=r_cell
        r_cell2=tmporig[nn+1]
        r_cell=np.concatenate([r_cell1,r_cell2])

        i_poynting1=i_poynting
        i_poynting2=tmpvec[nn+1]
        i_poynting=np.concatenate([i_poynting1,i_poynting2])

        E_XYZ1 = E_XYZ
        E_XYZ2 = tmpE[nn + 1]
        E_XYZ = np.concatenate([E_XYZ1, E_XYZ2])

        normal1 = normal
        normal2 = tmpN[nn + 1]
        normal = np.concatenate([normal1, normal2])

        triINDEX1 = triINDEX
        triINDEX2 = tmptri[nn + 1]
        triINDEX = np.concatenate([triINDEX1, triINDEX2])
    r_cell = np.ascontiguousarray(r_cell)
    i_poynting = np.ascontiguousarray(i_poynting)
    E_XYZ = np.ascontiguousarray(E_XYZ)
    normal = np.ascontiguousarray(normal)
    triINDEX = np.ascontiguousarray(triINDEX).flatten()
else:
    print('追踪次数不足',flush=True)

H_XYZ = 1/Z0*np.cross(i_poynting,E_XYZ)
coecos = np.sum(normal*i_poynting,axis=1)
tbl_constant=1.06
da = np.ones_like(cellarea_all[triINDEX])*tbl_constant#/coecos#####*
print('-----远场计算开始-----',flush=True)
from nearTofar_new import calculate_FarField_nb
#   注意：调用cupy计算要安装对应的库，安装指令如下
#   conda install -c conda-forge cupy

Es,Es_abs = calculate_FarField_nb(theta,phi,r_cell,da,i_poynting,E_XYZ,H_XYZ,f0)
Es = Es.transpose(1, 0, 2)
Es_abs = Es_abs.T

outputpath=content["output"]["result_path"]
del_file(outputpath)
np.save(outputpath+'/farField.npy',Es)
np.save(outputpath+'/farFieldAbs.npy',Es_abs)
np.save(outputpath + '/theta.npy', theta)
np.save(outputpath + '/phi.npy', phi)

plt.pcolor(Es_abs, cmap='rainbow')
# plt.savefig(outputpath+'/FarFieldsquare.jpg')
# plt.show()

avePY =  np.sum((pow(np.abs(Es[:,:,0]),2)+pow(np.abs(Es[:,:,1]),2)+pow(np.abs(Es[:,:,2]),2))*np.sin(theaxx)*dtheta*dphi*np.pi*np.pi/(np.pi*4*180*180))
dbiPY = 10*np.log10((pow(np.abs(Es[:,:,0]),2)+pow(np.abs(Es[:,:,1]),2)+pow(np.abs(Es[:,:,2]),2))/avePY)
dbiPY = dbiPY - 2 ###tbl_constant=2?
tbl_PY = pow(10,(np.max(dbiPY)-36.975)/10)*100
MAX = np.max(dbiPY)+1

bb1 = np.flip(dbiPY[int(angle_phi/2+90),1:numtheta])
bb2 = dbiPY[int(angle_phi/2),0:numtheta]
bb_E_dbi = np.concatenate((bb1,bb2),axis=0)

plt.pcolormesh(xxp,yyp,dbiPY, cmap ='jet',shading='gouraud');plt.colorbar()
plt.title('Gain of PY',fontsize = 14)
plt.xlabel('u',fontsize = 12);plt.ylabel('v',fontsize = 12)
plt.savefig(outputpath+'/farField.jpg')
# plt.show()

fig1 = plt.figure()
plt.plot(np.linspace(0-maxtheta,maxtheta,numtheta*2-1),bb_E_dbi, color = 'blue', linewidth = 2,linestyle='dashed')
plt.title('E-plane Gain',fontsize = 14)
plt.xlabel('Theta(deg)');plt.ylabel('Mag(dB)')
plt.xticks(np.linspace(0-maxtheta,maxtheta,11))
plt.xlim(0-maxtheta,maxtheta)
plt.ylim(MAX-35,MAX)
plt.grid()
plt.savefig(outputpath+'/FarFieldEplane.jpg')
# plt.show()

angle_phi = 90
bb1 = np.flip(dbiPY[int(angle_phi/2+90),1:numtheta])
bb2 = dbiPY[int(angle_phi/2),0:numtheta]
bb_H_dbi = np.concatenate((bb1,bb2),axis=0)

fig2=plt.figure()
plt.plot(np.linspace(0-maxtheta,maxtheta,numtheta*2-1),bb_H_dbi, color = 'blue', linewidth = 2,linestyle='dashed')
plt.title('H-plane Gain',fontsize = 14)
plt.xlabel('Theta(deg)');plt.ylabel('Mag(dB)')
plt.xticks(np.linspace(0-maxtheta,maxtheta,11))
plt.xlim(0-maxtheta,maxtheta)
plt.ylim(MAX-35,MAX)
plt.grid()
plt.savefig(outputpath+'/FarFieldHplane.jpg')
# plt.show()

radome = np.load(outputpath + '/farFieldAbs.npy')
ra_max = np.amax(radome)
outputpath_non = content["output"]["nonradome_path"]
del_file(outputpath_non)
np.save(outputpath_non + '/theta.npy', theta)
np.save(outputpath_non + '/phi.npy', phi)
if content["output"]["is_calc_nonradome"]==1:
    H_XYZ_inner = 1 / Z0 * np.cross(incvec, incfield)
    calculate_nonFarField_new(incfield, H_XYZ_inner, incorig, incarea_cos, incvec, maxtheta, numtheta, maxphi, numphi, f0, outputpath_non)

non = np.load(outputpath_non + '/farFieldAbs.npy')
non_max = np.amax(non)
tbl = (ra_max / non_max) * 100
np.save(outputpath_non + '/tbl.npy', tbl)
print('透波率：', tbl, '%',flush=True)

status_file = open(outputpath + '/status.txt','w')
#向文件中输入字符串
status_file.write('1\n')
status_file.close()

