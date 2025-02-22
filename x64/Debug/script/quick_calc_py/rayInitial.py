import numpy as np
import sys
import raytracingRT
import RTfuncs

sys.path.append(".") #把上级目录加入模块搜索

def rayInit(incfield,incorig,incvec,incINDEX,incarea,norm_pts,allLayerInfo,allmedia,k0,maxdepth,numofLayer,numofRay, threads_per_block):
    # 过滤0方向矢量
    num = len(incvec)
    x=np.arange(0,num)

    # 定义反射折射光线属性数组
    refvec = np.zeros((num, 3))
    reffield = np.zeros((num, 3),dtype=complex)
    reforig = np.zeros((num, 3))

    travec = np.zeros((num, 3))
    trafield = np.zeros((num, 3),dtype=complex)
    traorig = np.zeros((num,3))
    # 全局坐标系
    x0 = [1., 0., 0.]
    y0 = [0., 1., 0.]
    z0 = [0., 0., 1.]

    selectmedia=incINDEX[int(num*0.5)] #取第一批入射的某个面元的媒质参数
    m1_mu = np.ascontiguousarray(allmedia[selectmedia, 0])
    m1_eps = np.ascontiguousarray(allmedia[selectmedia, 1])
    m2_mu = np.ascontiguousarray(allmedia[selectmedia, 2])
    m2_eps = np.ascontiguousarray(allmedia[selectmedia, 3])
    k1=np.sqrt(m1_mu * m1_eps)*k0
    k2=np.sqrt(m2_mu * m2_eps)*k0
    yita1 = np.sqrt(m1_mu / m1_eps)
    yita2 = np.sqrt(m2_mu / m2_eps)
    n1=np.sqrt(m1_mu * m1_eps)
    n2=np.sqrt(m2_mu * m2_eps)
    incN=RTfuncs.matnormalize(norm_pts)

    inctheta=np.zeros(num,dtype=float)
    tratheta=np.zeros(num,dtype=float)
    arryita=np.zeros((num,2),dtype=complex)
    arrn = np.zeros((num, 2), dtype=complex)
    incfieldV = np.zeros(num, dtype=complex)
    incfieldP = np.zeros(num, dtype=complex)
    normrefvecV = np.zeros((num, 3), dtype=float)
    normrefvecP = np.zeros((num, 3), dtype=float)

    cosm1=np.sum(incN*incvec,axis=1)
    k1INDEX = np.argwhere(cosm1 >= 0).ravel()
    k2INDEX = np.argwhere(cosm1 < 0).ravel()
    inctheta[k1INDEX] = np.arccos(cosm1[k1INDEX])
    inctheta[k2INDEX] = np.pi - np.arccos(cosm1[k2INDEX])
    #设置媒质参数
    arryita[k1INDEX, 0] = yita1
    arryita[k1INDEX, 1] = yita2
    arryita[k2INDEX, 0] = yita2
    arryita[k2INDEX, 1] = yita1
    arrn[k1INDEX, 0] = n1
    arrn[k1INDEX, 1] = n2
    arrn[k2INDEX, 0] = n2
    arrn[k2INDEX, 1] = n1
    incN[k2INDEX]=incN[k2INDEX]-incN[k2INDEX]*2

    # 筛选正入射，不参与极化分解
    incvecV=np.cross(incN,incvec)
    absincvecV=np.linalg.norm(incvecV,axis=1)
    NincINDEX=np.argwhere(abs(1-abs(cosm1)) < 1e-5).ravel()
    #正入射
    reffield[NincINDEX], trafield[NincINDEX], refvec[NincINDEX], travec[NincINDEX] = RTfuncs.arrNormalInc(incfield[NincINDEX], incvec[NincINDEX], arryita[NincINDEX])

    # 极化分解
    xRT=np.arange(0,num)
    obINDEX=list(set(xRT).difference(set(NincINDEX)))#不是正入射的所有射线
    incvecP=np.cross(incvec,incvecV)
    normincvecV = RTfuncs.matnormalize(incvecV[obINDEX])
    normincvecP = RTfuncs.matnormalize(incvecP[obINDEX])
    incfieldV[obINDEX] = np.sum(incfield[obINDEX] * normincvecV, axis=1)
    incfieldP[obINDEX] = np.sum(incfield[obINDEX] * normincvecP, axis=1)
    incNtile1 = np.tile(np.array([np.sum(incvec * incN, axis=1)]).transpose(), (1, 3))
    refvec0 = incvec - 2 * incN * incNtile1
    refvecV = np.cross(incN, refvec0)
    refvecP = np.cross(refvec0, refvecV)
    normrefvecV[obINDEX] = RTfuncs.matnormalize(refvecV[obINDEX])
    normrefvecP[obINDEX] = RTfuncs.matnormalize(refvecP[obINDEX])
    #全反射处理
    edge_ang = np.real(np.arcsin(n1 / n2))
    tirINDEX0=np.argwhere(inctheta>edge_ang).ravel()
    tirINDEX=list(set(k2INDEX).intersection(set(tirINDEX0)))    #光密到光疏中所有大于临界角的射线索引
    coetir_v = (arryita[tirINDEX, 1] * np.cos(inctheta[tirINDEX]) + arryita[tirINDEX, 0] * np.sqrt(
        np.square(arrn[tirINDEX, 0] / arrn[tirINDEX, 1]) * np.square(np.sin(inctheta[tirINDEX])) - 1) * 1j) / (
                       arryita[tirINDEX, 1] * np.cos(inctheta[tirINDEX]) - arryita[tirINDEX, 0] * np.sqrt(
                   np.square(arrn[tirINDEX, 0] / arrn[tirINDEX, 1]) * np.square(np.sin(inctheta[tirINDEX])) - 1) * 1j)
    coetir_p = (arryita[tirINDEX, 0] * np.cos(inctheta[tirINDEX]) + arryita[tirINDEX, 1] * np.sqrt(
        np.square(arrn[tirINDEX, 0] / arrn[tirINDEX, 1]) * (1 - np.square(np.cos(inctheta[tirINDEX]))) - 1) * 1j) / (
                       arryita[tirINDEX, 0] * np.cos(inctheta[tirINDEX]) - arryita[tirINDEX, 1] * np.sqrt(
                   np.square(arrn[tirINDEX, 0] / arrn[tirINDEX, 1]) * (
                           1 - np.square(np.cos(inctheta[tirINDEX]))) - 1) * 1j)
    reffieldV = np.tile(np.array([incfieldV[tirINDEX] * coetir_v]).transpose(), (1, 3))
    reffieldP = np.tile(np.array([incfieldP[tirINDEX] * coetir_p]).transpose(), (1, 3))
    reffield[tirINDEX] = reffieldV * normrefvecV[tirINDEX] + reffieldP * normrefvecP[tirINDEX]
    trafield[tirINDEX] = [0., 0., 0.]
    refvec[tirINDEX] = refvec0[tirINDEX]
    travec[tirINDEX] = [0., 0., 0.]

    #正常反射折射光线
    RTINDEX = list(set(obINDEX).difference(set(tirINDEX)))
    tratheta[RTINDEX] = np.real(np.arcsin(np.sin(inctheta[RTINDEX]) * arrn[RTINDEX,0] / arrn[RTINDEX,1]))
    travec0 = incvec[RTINDEX] - incN[RTINDEX] * np.tile(
        np.array([np.sum(incvec[RTINDEX] * incN[RTINDEX], axis=1)]).transpose(), (1, 3)) + incN[RTINDEX] * np.tile(
        np.array([(np.sin(inctheta[RTINDEX]) / np.tan(tratheta[RTINDEX]))]).transpose(), (1, 3))
    normtravec = RTfuncs.matnormalize(travec0)
    travecV=np.cross(incN[RTINDEX], travec0)
    travecP=np.cross(travec0, travecV)
    normtravecV = RTfuncs.matnormalize(travecV)
    normtravecP = RTfuncs.matnormalize(travecP)
    coer_p, coer_v = RTfuncs.arrref(arryita[RTINDEX,0], arryita[RTINDEX,1], inctheta[RTINDEX], tratheta[RTINDEX])
    coet_p, coet_v = RTfuncs.arrtra(arryita[RTINDEX,0], arryita[RTINDEX,1], inctheta[RTINDEX], tratheta[RTINDEX])
    RTreffieldV = np.tile(np.array([incfieldV[RTINDEX] * coer_v]).transpose(), (1, 3))
    RTreffieldP = np.tile(np.array([incfieldP[RTINDEX] * coer_p]).transpose(), (1, 3))
    RTtrafieldV = np.tile(np.array([incfieldV[RTINDEX] * coet_v]).transpose(), (1, 3))
    RTtrafieldP = np.tile(np.array([incfieldP[RTINDEX] * coet_p]).transpose(), (1, 3))
    reffield[RTINDEX] = RTreffieldV * normrefvecV[RTINDEX] + RTreffieldP * normrefvecP[RTINDEX]
    trafield[RTINDEX] = RTtrafieldV * normtravecV + RTtrafieldP * normtravecP
    refvec[RTINDEX]=refvec0[RTINDEX]
    travec[RTINDEX]=normtravec

    cnt=-1

    # raytracingRT.rayRT(reffield, incorig, refvec, incINDEX, incarea,allLayerInfo,allmedia,k0, maxdepth, maxdepth)
    raytracingRT.rayRT(trafield, incorig, travec, incINDEX, incarea,allLayerInfo,allmedia,k0, maxdepth, maxdepth,numofLayer,numofRay,threads_per_block)









