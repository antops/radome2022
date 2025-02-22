import numpy as np
import sys
import os
import rayTracing
import RTfuncs

sys.path.append(".") #把上级目录加入模块搜索


cnt=-1

def rayRT(incfield,incorig,incvec,incINDEX,incarea,allLayerInfo,allmedia,k0,depth,maxDepth,numofLayer,numofRay, threads_per_block):
    syspath = os.path.dirname(os.path.realpath(sys.argv[0]))
    norm_tri = np.ascontiguousarray(allLayerInfo[:, :3])
    Points = np.ascontiguousarray(allLayerInfo[:, 3:12])
    layerID=np.ascontiguousarray(allLayerInfo[:, 12:]).flatten()
    stopTraceflg = 0
    # 过滤底面边缘的0方向矢量
    vecabs = np.linalg.norm(incvec,axis=1)
    IndexVec = np.argwhere(vecabs > 1e-7).ravel()
    incorig=incorig[IndexVec]
    incvec=incvec[IndexVec]
    incfield=incfield[IndexVec]
    incINDEX=incINDEX[IndexVec]
    incarea=incarea[IndexVec]
    len0 = len(incvec)
    x=np.arange(0,len0)
    # 追踪
    tri_INDEX, ray_INDEX, intersection, T = rayTracing.ray_tracing(incorig, incvec, Points, threads_per_block)
    global cnt
    print('current depth= ', depth,' 相交射线数量=', len(ray_INDEX),flush=True)
    # miss_tri=list(set(x).difference(set(ray_INDEX)))
    miss_INDEX = list(set(x).difference(set(ray_INDEX)))
    miss_INDEX=np.array(miss_INDEX)
    if depth!=maxDepth and len(miss_INDEX) != 0:
        # missNz = norm_tri[incINDEX[miss_INDEX]][:,2]
        # Nfilter = np.argwhere(missNz < 0).ravel()
        misslayerID = layerID[incINDEX[miss_INDEX]]
        outerINDEX = np.argwhere(misslayerID < 0.1).ravel()
        if len(outerINDEX) > numofRay/10:
            cnt = cnt + 1
            missfield = incfield[miss_INDEX[outerINDEX]]
            missorig = incorig[miss_INDEX[outerINDEX]]
            missvec=incvec[miss_INDEX[outerINDEX]]
            misscellN=norm_tri[incINDEX[miss_INDEX[outerINDEX]]]
            misscellINDEX=incINDEX[miss_INDEX[outerINDEX]]
            misscellarea=incarea[miss_INDEX[outerINDEX]]
            np.save((syspath + "\\4Nearfield\\missfield" + str(cnt) + ".npy"), missfield)
            np.save((syspath + "\\4Nearfield\\missorig" + str(cnt) + ".npy"), missorig)
            np.save((syspath + "\\4Nearfield\\missvec" + str(cnt) + ".npy"), missvec)
            np.save((syspath + "\\4Nearfield\\misscellN" + str(cnt) + ".npy"),misscellN)
            np.save((syspath + "\\4Nearfield\\misscellINDEX" + str(cnt) + ".npy"), misscellINDEX)
            np.save((syspath + "\\4Nearfield\\misscellarea" + str(cnt) + ".npy"), misscellarea)
    if depth != maxDepth and len(ray_INDEX) != 0:
        origlayerID = layerID[incINDEX[ray_INDEX]]
        hitlayerID = layerID[tri_INDEX]
        inner_origINDEX = np.argwhere(origlayerID > numofLayer - 1.1).ravel()
        inner_hitINDEX = np.argwhere(hitlayerID > numofLayer - 1.1).ravel()
        if len(inner_origINDEX)>numofRay/35 and len(inner_hitINDEX)>numofRay/35:
            stopTraceflg=1
    incorig = incorig[ray_INDEX]
    incvec = RTfuncs.matnormalize(incvec[ray_INDEX])
    incfield = incfield[ray_INDEX]
    incINDEX = incINDEX[ray_INDEX]
    incarea = incarea[ray_INDEX]
    num = len(incvec)
    # 定义反射折射光线属性数组
    reftri_INDEX = np.zeros(num,dtype=int)
    refvec = np.zeros((num, 3))
    reffield = np.zeros((num, 3),dtype=complex)
    reforig = np.zeros((num, 3))
    refarea = np.zeros(num,dtype=float)

    tratri_INDEX = np.zeros(num,dtype=int)
    travec = np.zeros((num, 3))
    trafield = np.zeros((num, 3),dtype=complex)
    traorig = np.zeros((num,3))
    traarea = np.zeros(num, dtype=float)
    # 全局坐标系
    x0 = [1., 0., 0.]
    y0 = [0., 1., 0.]
    z0 = [0., 0., 1.]

    mu1 = np.ascontiguousarray(allmedia[:, 0])
    eps1 = np.ascontiguousarray(allmedia[:, 1])
    mu2 = np.ascontiguousarray(allmedia[:, 2])
    eps2 = np.ascontiguousarray(allmedia[:, 3])
    mu1.flatten()
    eps1.flatten()
    mu2.flatten()
    eps2.flatten()
    mu1=mu1[tri_INDEX]
    eps1=eps1[tri_INDEX]
    mu2 = mu2[tri_INDEX]
    eps2 = eps2[tri_INDEX]
    k1 = np.sqrt(mu1 * eps1) * k0
    k2 = np.sqrt(mu2 * eps2) * k0
    yita1 = np.sqrt(mu1 / eps1)
    yita2 = np.sqrt(mu2 / eps2)
    n1 = np.sqrt(mu1 * eps1)
    n2 = np.sqrt(mu2 * eps2)
    incN=RTfuncs.matnormalize(norm_tri[tri_INDEX])

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
    arryita[k1INDEX, 0] = yita1[k1INDEX]
    arryita[k1INDEX, 1] = yita2[k1INDEX]
    arryita[k2INDEX, 0] = yita2[k2INDEX]
    arryita[k2INDEX, 1] = yita1[k2INDEX]
    arrn[k1INDEX, 0] = n1[k1INDEX]
    arrn[k1INDEX, 1] = n2[k1INDEX]
    arrn[k2INDEX, 0] = n2[k2INDEX]
    arrn[k2INDEX, 1] = n1[k2INDEX]
    incN[k2INDEX]=incN[k2INDEX]-incN[k2INDEX]*2
    exptile1 = np.tile(np.array([np.exp((-1j) * T[k1INDEX] * k1[k1INDEX])]).transpose(), (1, 3))
    exptile2 = np.tile(np.array([np.exp((-1j) * T[k2INDEX] * k2[k2INDEX])]).transpose(), (1, 3))
    incfield[k1INDEX] = incfield[k1INDEX] * exptile1
    incfield[k2INDEX] = incfield[k2INDEX] * exptile2

    # 筛选正入射，不参与极化分解
    incvecV=np.cross(incN,incvec)
    absincvecV=np.linalg.norm(incvecV,axis=1)
    NincINDEX=np.argwhere(abs(1-abs(cosm1)) < 1e-5).ravel()
    #正入射
    reffield[NincINDEX], trafield[NincINDEX], refvec[NincINDEX], travec[NincINDEX] = RTfuncs.arrNormalInc(incfield[NincINDEX], incvec[NincINDEX], arryita[NincINDEX])
    reforig[NincINDEX]=intersection[NincINDEX]
    refarea[NincINDEX]=incarea[NincINDEX]
    reftri_INDEX[NincINDEX]=tri_INDEX[NincINDEX]
    traorig[NincINDEX] = intersection[NincINDEX]
    traarea[NincINDEX] = incarea[NincINDEX]
    tratri_INDEX[NincINDEX] = tri_INDEX[NincINDEX]
    # print('正入射数量：',len(NincINDEX),'射线总数：',num)
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
    reforig[tirINDEX] = intersection[tirINDEX]
    refarea[tirINDEX] = incarea[tirINDEX]
    reftri_INDEX[tirINDEX] = tri_INDEX[tirINDEX]
    traorig[tirINDEX] = intersection[tirINDEX]
    tratri_INDEX[tirINDEX] = tri_INDEX[tirINDEX]

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
    reforig[RTINDEX] = intersection[RTINDEX]
    refarea[RTINDEX] = incarea[RTINDEX]
    reftri_INDEX[RTINDEX] = tri_INDEX[RTINDEX]
    traorig[RTINDEX] = intersection[RTINDEX]
    traarea[RTINDEX] = incarea[RTINDEX]
    tratri_INDEX[RTINDEX] = tri_INDEX[RTINDEX]

    depth = depth - 1
    numref = len(NincINDEX)+len(tirINDEX)+len(RTINDEX)
    numtra = len(NincINDEX)+len(RTINDEX)
    if depth==0:
        return 0
    else:
        if numref > 1000 and stopTraceflg==0:
            rayRT(reffield, reforig, refvec, reftri_INDEX, refarea, allLayerInfo,allmedia,k0, depth,maxDepth,numofLayer,numofRay, threads_per_block)
        if numtra > 1000 and stopTraceflg==0:
            rayRT(trafield, traorig, travec, tratri_INDEX, traarea, allLayerInfo,allmedia,k0, depth,maxDepth,numofLayer,numofRay, threads_per_block)









