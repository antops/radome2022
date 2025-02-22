import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
from initial_input import initData
sys.path.append(".")  # 把上级目录加入模块搜索

version=1
#jsopath = 'meta_autoCurve.json'
jsopath = sys.argv[1]
content = initData(jsopath)
if content["version"] != version:
    print('version error', flush=True)
    sys.exit()

n_layer=content["n"]
sampling=content["sampling"]#采样点数，默认1500
current=0
tip_r=10 #最外层球头半径默认为10mm
#结合8院给的算例，馈源所在平面距离天线罩底面0.075m，生成的天线罩需要截断
for layer in range(1,n_layer+2):
# layer=3 #当前生成第几层，最外层为0固定，内层为3
        #各层厚度，mm
        # para=27
        linepath=content["result_path"]

        vclinez=np.zeros(sampling)
        vclinex=np.zeros(sampling)
        start=0.0 + current/1000 #参数a
        bottom=0.567 #len
        step=(bottom-start)/sampling
        rr=0.1016-current/1000 #参数b
        threshold=rr/np.sqrt(np.arccos(-1))
        for ii in range(sampling):
            vclinez[ii]=start+step*ii
            theta=np.arccos(1-(2*step*ii/(bottom-start)))
            vclinex[ii]=threshold*np.sqrt(theta-0.5*np.sin(2*theta))

        tip_R=tip_r/1000 #球头半径，毫米，参数r
        print('a=',start,'b=',rr,'r=',tip_R)
        R_scan=int(sampling/10) #总长度的十分之一，求这一段每个离散点对应的球头半径和圆心
        scanz=vclinez[0:R_scan]
        scanx=vclinex[0:R_scan]
        grad=np.gradient(scanx,scanz) #每一点切线斜率
        scantip_o=grad*scanx+scanz #法线与横轴z交点
        scantip_R=np.sqrt(scanx**2+(scantip_o-scanz)**2) #截距，即球头半径
        ffR=interp1d(scantip_R,scantip_o) #插值搜索
        ffz=interp1d(scantip_R,scanz)
        tip_o=ffR(tip_R)
        tip_z=ffz(tip_R) #切点横坐标
        #画图，3/8圆
        tipx=np.arange(tip_o-tip_R,tip_o+0.5*tip_R,step)
        tipy=np.sqrt(tip_R**2-(tipx-tip_o)**2)

        tipnum=int((bottom-tip_o+tip_R)/step)+1
        prt1=int((tip_z-(tip_o-tip_R))/step)+1
        prt2=tipnum-prt1
        points=np.zeros([tipnum,3])
        points[0:prt1,0]=tipy[0:prt1]
        points[prt1:,0]=vclinex[(sampling-tipnum+prt1):]
        points[0:prt1,2]=bottom-tipx[0:prt1]
        points[prt1:,2]=bottom-vclinez[(sampling-tipnum+prt1):]

        np.savetxt(linepath+str(layer-1)+'.txt',points,fmt='%.7f')


        if layer != n_layer+1:
                current=current+content["model_meta"][layer-1]["para"]
                tip_r=content["model_meta"][layer-1]["r"]


