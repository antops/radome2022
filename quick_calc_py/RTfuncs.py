import numpy as np
import vtkmodules.all as vtk
import sys

sys.path.append(".") #把上级目录加入模块搜索

def arrref(m1_yita, m2_yita, inc_theta, tra_theta):   #批量反射系数计算
    coer_p = (m1_yita * np.cos(inc_theta) - m2_yita * np.cos(tra_theta)) / (
            m1_yita * np.cos(inc_theta) + m2_yita * np.cos(tra_theta))  # 反射强度（TM）
    coer_v = (m2_yita * np.cos(inc_theta) - m1_yita * np.cos(tra_theta)) / (
            m2_yita * np.cos(inc_theta) + m1_yita * np.cos(tra_theta))  # 反射强度（TE）
    return coer_p, coer_v

def arrtra(m1_yita, m2_yita, inc_theta, tra_theta):   #透射系数计算
    coet_p = (2*m2_yita * np.cos(tra_theta)) / (
            m1_yita * np.cos(inc_theta) + m2_yita * np.cos(tra_theta))  # 折射强度(TM)
    coet_v = (2*m2_yita * np.cos(inc_theta)) / (
            m2_yita * np.cos(inc_theta) + m1_yita * np.cos(tra_theta))  # 折射强度(TE)
    return coet_p, coet_v

def arrNormalInc(incfield,incvec,yita):   #批量处理正入射
    coer = (yita[:,0] -yita[:,1]) / (yita[:,1] + yita[:,0])
    coet = (2 * yita[:,1]) / (yita[:,1] + yita[:,0])
    coettile = np.tile(np.array([coet]).transpose(), (1, 3))
    coertile = np.tile(np.array([coer]).transpose(), (1, 3))
    ref_field_out = incfield * coertile
    tra_field_out = incfield * coettile
    ref_vec = incvec - 2 * incvec
    tra_vec = incvec
    return ref_field_out,tra_field_out,ref_vec,tra_vec

def cellFieldCal(missInfo,cellField):   #场值赋给面元
    cell = list(set(missInfo.tri_INDEX))    #不重复叠加
    cellall = list(missInfo.tri_INDEX)
    numcls = len(cell)
    num = len(missInfo.field)
    for i in range(numcls):
        index = cellall.index(cell[i], 0, num)
        cellField[missInfo.tri_INDEX[index]]=cellField[missInfo.tri_INDEX[index]]+missInfo.field[index]
    return cellField

def arrcellFieldCal(triINDEX,field,cellField):   #批量场值赋给面元
    # cell = list(set(triINDEX))    #不重复叠加
    # cellall = list(triINDEX)
    # numcls = len(cell)
    numcls = len(triINDEX)
    # num = len(missInfo.field)
    for i in range(numcls):
        # index = cellall.index(cell[i], 0, num)
        cellField[triINDEX]=cellField[triINDEX]+field
    return cellField

def pointFieldCal(missInfo,rtpPolydata,pointField):   #场值赋给节点
    cell=list(set(missInfo.tri_INDEX))
    cellall=list(missInfo.tri_INDEX)
    num = len(missInfo.field)
    numcls=len(cell)
    print(num,numcls)
    cpIds = vtk.vtkIdList()
    for i in range(numcls):
        index=cellall.index(cell[i],0,num)
        rtpPolydata.GetCellPoints(missInfo.tri_INDEX[index], cpIds)
        for j in range(cpIds.GetNumberOfIds()):
            pointField[cpIds.GetId(j)]=pointField[cpIds.GetId(j)]+missInfo.field[index]*1/3
    return pointField

def arrpointFieldCal(triINDEX,field,rtpPolydata,pointField):   #批量场值赋给节点
    # cell=list(set(missInfo.tri_INDEX))
    # cellall=list(missInfo.tri_INDEX)
    # num = len(missInfo.field)
    numcls=len(triINDEX)
    # print(num,numcls)
    cpIds = vtk.vtkIdList()
    for i in range(numcls):
        # index=cellall.index(cell[i],0,num)
        rtpPolydata.GetCellPoints(triINDEX, cpIds)
        for j in range(cpIds.GetNumberOfIds()):
            pointField[cpIds.GetId(j)]=pointField[cpIds.GetId(j)]+field*1/3 #系数待定
    return pointField

def pointIncFieldCal(incInfo,rtpPolydata,pointField):   #场值赋给节点(内壁)
    num = len(incInfo.field)
    cpIds = vtk.vtkIdList()
    for i in range(num):
        rtpPolydata.GetCellPoints(incInfo.tri_INDEX[i], cpIds)
        for j in range(cpIds.GetNumberOfIds()):
            pointField[cpIds.GetId(j)]=pointField[cpIds.GetId(j)]+incInfo.field[i]*1/3
    return pointField

def matnormalize(mat):  #批量向量归一化
    x,y=np.shape(mat)
    coe=np.linalg.norm(mat,axis=1)
    coetile = np.tile(np.array([coe]).transpose(), (1, y))
    return mat/coetile