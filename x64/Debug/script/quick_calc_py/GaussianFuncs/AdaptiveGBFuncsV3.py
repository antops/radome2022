# -*- coding: utf-8 -*-
"""
Created on Tue Apr 12 12:34:50 2022
https://blog.csdn.net/weixin_40744368/article/details/102740932
@author: baiming
Version 2.0
高斯函数集
"""
import numpy as np
import matplotlib.pyplot as plt
import time
# from progressbar import progressbar
#from sklearn.preprocessing import normalize

def GaussianEyZ0(x0,y0,wx,wy,Xmesh,Ymesh): #物理距离
    return np.exp(-(Xmesh-x0)*(Xmesh-x0)/wx/wx-(Ymesh-y0)*(Ymesh-y0)/wy/wy) 

def GaussianXYZ(k0,x0,y0,z0,w0x,w0y,X,Y,Z, CalcFlag): #计算点集，按坐标矩阵形式输入，返回所有点电和磁场，一维矩阵索引形式
    XxXx=(X-x0) *(X-x0)# （16920，） XxXx阵列
    YyYy=(Y-y0) *(Y-y0)# （16920，） YyYy阵列
    Z=Z-z0
    ZZ=Z*Z # （16920，）YyYy阵列
    Zx=k0 * w0x * w0x *0.5 #PiW0xW0xlamda=np.pi*w0x*w0x/lamda
    Zy=k0 * w0y * w0y *0.5#PiW0yW0ylamda=np.pi*w0y*w0y/lamda
    ZxZx,ZyZy=Zx*Zx,Zy*Zy
    
    Wx = w0x * np.sqrt(1 + ZZ / ZxZx) #沿Z轴传播一定距离后的高斯波束的束腰w(wx,wy通用)
    Wy = w0y * np.sqrt(1 + ZZ / ZyZy)
    R1x = Z / (ZZ + ZxZx) #沿Z轴传播一定距离后的高斯波束的曲率半径R1(R1x,R1y通用)
    R1y = Z / (ZZ + ZyZy)
    phi = 0.5 * (np.arctan(Z /Zx) + np.arctan(Z / Zy)) #返回相位值Phi
    F2F2 = 1 + XxXx * R1x * R1x + YyYy * R1y * R1y #修正参数F2
    Amplitude = np.sqrt(w0x * w0y / Wx / Wy) / F2F2
    Phase0 = -k0 * (Z + XxXx * R1x *0.5 + YyYy * R1y *0.5) + phi
    
    Empty=np.zeros(X.shape,dtype=complex)
    (Ex,Ey,Ez,Hx,Hy,Hz)=(Empty,Empty,Empty,Empty,Empty,Empty) #先全部置0
    #电场：Ex,Ey独立，由IsEy来决定
    if CalcFlag[0]==1: Ex = Amplitude * np.exp(-XxXx / F2F2 / (Wx * Wx) - YyYy / F2F2 / (Wy * Wy)+1j*Phase0) 
    if CalcFlag[1]==1: Ey = Amplitude * np.exp(-XxXx / F2F2 / (Wx * Wx) - YyYy / F2F2 / (Wy * Wy)+1j*Phase0)
    if CalcFlag[2]==1: Ez=-(X-x0)/(Z+1j*Zx)*Ex-(Y-y0)/(Z+1j*Zy)*Ey  #交叉极化Ez(由Ex,Ey贡献)
    #磁场：
    if CalcFlag[3]==1: Hx=-1/(120*np.pi)*Ey + 1/(120*np.pi)*(-(X-x0)*(Y-y0))/2/(Z+1j*Zx)/(Z+1j*Zy)*Ex #Ex的贡献是小量-50dB
    if CalcFlag[4]==1: Hy= 1/(120*np.pi)*Ex - 1/(120*np.pi)*(-(X-x0)*(Y-y0))/2/(Z+1j*Zx)/(Z+1j*Zy)*Ey #Ey的贡献是小量-50dB
    if CalcFlag[5]==1: Hz=-(X-x0)/(Z+1j*Zx)*Hx -(Y-y0)/(Z+1j*Zy)*Hy #交叉极化Hz(由Hx,Hy贡献)
    FieldXYZ=np.array([Ex,Ey,Ez,Hx,Hy,Hz])
    return FieldXYZ # (6,shape(X/Y/Z)) (6,16900)

def GaussianThetaPhi(k0,x0,y0,z0,w0x,w0y,theta,phi,CalcFlag): #计算点集，按坐标矩阵形式输入，返回所有点电和磁场，一维矩阵索引形式
    SinTheta=np.sin(theta)
    SinPhi = np.sin(phi)
    CosTHeta=np.cos(theta)
    CosPhi  =np.cos(phi)
    CenterProjection=x0*SinTheta*CosPhi+y0*SinTheta*SinPhi+z0*CosTHeta
    Amplitude=1/(4*np.pi)*w0x*w0y
    Phase=np.exp(1j*k0*CenterProjection)
    Gauss=np.exp(-k0*k0/4*SinTheta*SinTheta*(CosPhi*CosPhi*w0x*w0x + SinPhi*SinPhi*w0y*w0y))
    
    Empty=np.zeros(theta.shape,dtype=complex)
    (Ex,Ey)=(Empty,Empty) #先全部置0
    #直角坐标系电场：Ex,Ey各自独立设定
    if CalcFlag[0]==1: Ex = Amplitude * Phase* Gauss
    if CalcFlag[1]==1: Ey = Amplitude * Phase* Gauss
    #球坐标系电场：Etheta, Ephi
    Etheta= 1j*np.pi*k0*(CosPhi*Ex+SinPhi*Ey)    
    EPhi  = 1j*np.pi*k0*(CosPhi*Ey-SinPhi*Ex)*CosTHeta

    Farfield=np.array([Ex,Ey,Etheta,EPhi])
    return Farfield # (4,shape(X/Y/Z)) (4,16900)

def AdaptiveDecomposition(FieldToDecom, GaussInfoFileName,dx,dy): #****分解复杂场分布**************
    start=time.time()
    print("Field Decomposition......")
    # FieldLeft=FieldToDecom[:,:,1]
    FieldLeft = FieldToDecom
    (Xrange,Yrange)=FieldLeft.shape
    Xmin,Xmax=-(Xrange-1)/2*dx,(Xrange-1)/2*dx
    Ymin,Ymax=-(Yrange-1)/2*dy,(Yrange-1)/2*dy
    XmeshIn,YmeshIn=np.mgrid[Xmin:Xmax:Xrange*1j,Ymin:Ymax:Yrange*1j]

    FittedGB=np.zeros(FieldLeft.shape,dtype=complex)
    Energy0 = np.sum(np.abs(FieldLeft*FieldLeft))
    # plt.contourf(np.abs(FieldLeft), 50, cmap='rainbow')
    # plt.show()
    Gauss_Info = np.zeros((0, 7))

    """
    n代表分解的数，源比较复杂时建议多分一点！
    """
    for n in np.arange(0, 500):
    # for n in range(2):
        Amp=np.abs(FieldLeft)     #Pha=np.angle(FieldLeft)
        # 确定极大值位置
        MaxLocation=np.unravel_index(Amp.argmax(), Amp.shape)
        LineX=Amp[:,MaxLocation[1]] # 极大值点所在的行
        LineY=Amp[MaxLocation[0],:] # 极大值点所在的列
        MinPointsX = (np.diff(np.sign(np.diff(LineX, 2))) > 0).nonzero()[0] + 1 # //X截线上极小值的位置（利用二次导数变号）
        MinPointsY = (np.diff(np.sign(np.diff(LineY, 2))) > 0).nonzero()[0] + 1 # //Y截线上极小值的位置（利用二次导数变号）
        # 确定束腰大小
        Wx = np.sort(np.abs(MinPointsX - MaxLocation[0])) # //线上极小值的点们距离极大值的距离集合
        Wy = np.sort(np.abs(MinPointsY - MaxLocation[1])) # //线上极小值的点们距离极大值的距离集合
        Wx = (Wx[0]+Wx[1])*0.5*dx if Wx.shape[0] >1 else Wx[0]*dx #前两个距离确定束腰（效果好），有可能点数不够
        Wy = (Wy[0]+Wy[1])*0.5*dy if Wy.shape[0] >1 else Wy[0]*dy # Wy = Wy[np.nonzero(Wy)][0]*1.75 之前的做法
        # Wx = (Wx[0]+Wx[1])*0.5*dx if Wx.shape[0] >1 else Wx[0]*0.5*dx #前两个距离确定束腰（效果好），有可能点数不够
        # Wy = (Wy[0]+Wy[1])*0.5*dy if Wy.shape[0] >1 else Wy[0]*0.5*dy # Wy = Wy[np.nonzero(Wy)][0]*1.75 之前的做法
        X0,Y0 = (MaxLocation[0]-Xrange/2)*dx, (MaxLocation[1]-Yrange/2)*dy
        #获得拟合的高斯场
        FitGB=FieldLeft[MaxLocation]*GaussianEyZ0(X0,Y0,Wx,Wy,XmeshIn,YmeshIn) 
        Info=(X0,Y0,0,Wx,Wy,np.abs(FieldLeft[MaxLocation]),np.angle(FieldLeft[MaxLocation])) #只需记录这些信息
        #减去拟合高斯 #记录拟合高斯
        FieldLeft=FieldLeft-FitGB 
        # plt.contourf(np.abs(FieldLeft), 50, cmap='rainbow')
        # plt.show()
        FittedGB=FittedGB+FitGB   
        Gauss_Info = np.insert(Gauss_Info, 0, Info, axis= 0)
    Energy = np.sum(np.abs(FieldLeft*FieldLeft))
#    print(n, Energy/Energy0, Amp[MaxLocation])
    np.savetxt(GaussInfoFileName,np.flipud(Gauss_Info)) #反转保存
    print(n, "剩余能量占比:",Energy/Energy0*100,"% ", "Time:",time.time()-start)
    plt.pcolormesh(np.abs(FittedGB), cmap='jet')
    plt.colorbar()
    # plt.show()
    return

def AdaptiveDecompositionCurvSurf(k0,OriginField,SurfPtsWorld,RefAxisU,RefAxisV,RefAxisN,RefOrigin,GaussInfoFileName): #****自适应曲面分解**************
    start=time.time()
    lamda=2*np.pi/k0
    print("Field on Surface Decomposition......")
    
    #过滤掉较小值的曲面位置，大幅加速分解运算速度
    Max=np.max(np.abs(OriginField))
    Index=np.argwhere(np.abs(OriginField)/Max > 1e-3).ravel() # 过滤标准为1e-3
    OriginFieldFiltered=OriginField[Index]
    SurfPtsWorldFiltered=SurfPtsWorld[Index]
    #反射面的点转换到本地坐标系表示，是根据反射波建立的本地坐标系
    SurfPtsFitLocal=ToLocalCoordinate(SurfPtsWorldFiltered,RefAxisU,RefAxisV,RefAxisN,RefOrigin)
    FieldLeft=OriginFieldFiltered # E本地 Ex极化
    FittedGB=np.zeros(SurfPtsFitLocal.shape[0],dtype=complex) # (NumP,3) 准备好所有点,每个点3个分量
    Energy0 = np.sum(np.abs(FieldLeft*FieldLeft))
    (X,Y,Z)=(SurfPtsFitLocal[:,0],SurfPtsFitLocal[:,1],SurfPtsFitLocal[:,2])
    LossX=np.linspace(0,499,500,dtype=int)
    LossY=np.zeros(500)
    WRecord=np.zeros(500)
    Gauss_Info = np.zeros((0, 19)) # 若干信息
    
    for n in np.arange(0, 365):
    # 确定束腰大小
        # Wx=lamda*2.2
        # Wy=lamda*2.2
        # if n>=10:
        #     Wx=lamda*2.0
        #     Wy=lamda*2.0
        # if n>=50:
       
        Wx=lamda*0.8
        Wy=lamda*0.8
        if n>=200:
            Wx=lamda*0.6
            Wy=lamda*0.6
        # if (n>=300):
        #     Wx=lamda*0.4
        #     Wy=lamda*0.4
        
        # 确定极大值位置
        AmpE=np.abs(FieldLeft)     
        MaxLocationEu=AmpE.argmax()
        (X0,Y0,Z0) = SurfPtsFitLocal[MaxLocationEu] #坐标本身已经是物理尺寸
        #获得拟合的高斯场
        CalcFlag=np.array([1,0,0,0,0,0]) #  Ey=0 只算Ex
        FitGB=FieldLeft[MaxLocationEu]*GaussianXYZ(k0,X0,Y0,Z0,Wx,Wy,X,Y,Z,CalcFlag)[0,:] # 还是用Ex的函数
        Info=(X0,Y0,Z0, Wx,Wy,np.abs(FieldLeft[MaxLocationEu]),np.angle(FieldLeft[MaxLocationEu]),
              RefOrigin[0],RefOrigin[1],RefOrigin[2],RefAxisU[0],RefAxisU[1],RefAxisU[2],
              RefAxisV[0],RefAxisV[1],RefAxisV[2],RefAxisN[0],RefAxisN[1],RefAxisN[2]) 
        #减去拟合高斯 #记录拟合高斯
        FieldLeft=FieldLeft-FitGB
        FittedGB=FittedGB+FitGB   
        Gauss_Info = np.insert(Gauss_Info, 0, Info, axis= 0)
        Energy= np.sum(np.abs(FieldLeft*FieldLeft))
        print('\r', n, Energy/Energy0, AmpE[MaxLocationEu],Wx/lamda,end='',flush=True)
        LossY[n]=np.log(Energy/Energy0)
        WRecord[n]=Wx/lamda
    plt.plot(LossX,LossY)
    plt.plot(LossX,WRecord)
    # plt.show()
    np.savetxt(GaussInfoFileName,np.flipud(Gauss_Info)) #反转保存
    print("  Time:",time.time()-start)
    return

def FieldOnSurface(k0,GaussInfo,ptsCordis,FieldFileName,PoyntingFileName,CalcFlag): #*******计算曲面上完整场*****************
    print("Illuminating On target Surface Points .....") 
    start=time.time()
    Field=np.zeros((6, ptsCordis.shape[0]),dtype=(complex)) #ExEyEz+HxHyHz
    for n, Info in enumerate(GaussInfo): # 遍历所有高斯波单元
        Field=Field+Info[5]*np.exp(1j*Info[6])*GaussianXYZ(k0,Info[0],Info[1],Info[2],Info[3],Info[4],ptsCordis[:,0],ptsCordis[:,1],ptsCordis[:,2],CalcFlag)
        # print('\r', n, " of ", GaussInfo.shape[0], end='', flush=True)

    np.save(FieldFileName,Field.T) # (16900,6) joblib并行反效果

    FieldExEyEz=Field.T[:,:3] #电场部分 [16900,3]
    FieldHxHyHz=Field.T[:,3:] #磁场部分 [16900,3]????
    Poynting=np.cross(FieldExEyEz,np.conj(FieldHxHyHz)) #全局波印廷矢量 
    PoyntingVector=np.real(Poynting) #波印廷取实部  np.real(Poynting) !!!

    with np.errstate(divide='ignore',invalid='ignore'):
        num = len(PoyntingVector)
        PoyntingVector = np.true_divide(PoyntingVector,np.linalg.norm(PoyntingVector,axis=1).reshape((num,1)))
        PoyntingVector = np.nan_to_num(PoyntingVector)
    np.save(PoyntingFileName,PoyntingVector)
    print("\n","Surface Illuminated. Time:",time.time()-start)
    return  Field.T  #(16900,6)

def FieldReflectedFromSurface(IncidentEH,SurfNormals,ReflectedFileName): #*******计算从反射面反射回来的场*****************
    FieldExEyEz=IncidentEH[:,:3] #电场部分 [16900,3]
    FieldHxHyHz=IncidentEH[:,3:] #磁场部分 [16900,3]
    print("Field reflecting.....",end='')
    
    EVertical=np.sum(FieldExEyEz*SurfNormals,axis=1,keepdims=1) #3个场分量投影相加得法向电场标量 （NUmP,1）
    HVertical=np.sum(FieldHxHyHz*SurfNormals,axis=1,keepdims=1) #3个场分量投影相加得法向电场标量 （NUmP,1）
    EReflected=2*EVertical*SurfNormals-FieldExEyEz #所在位置的表面反射场
    HReflected=FieldHxHyHz-2*HVertical*SurfNormals #所在位置的表面反射场
   
    EHReflected=np.append(EReflected,HReflected,axis=1)
    np.save(ReflectedFileName,EHReflected) # (16900,6) joblib并行反效果
    print("Surface Reflection done.")
    return  EHReflected  #(16900,3)

def GaussToFarfield(k0,GaussInfoFileName,FarfieldFileName,Thetamesh,Phimesh): # 计算远场方向图
    print("Gauss To Farfield.....") 
    CalcFlag=np.array([0,1]) #  Ex=0 算Ey
    Farfield=np.zeros((4, Thetamesh.shape[0],Phimesh.shape[1]),dtype=(complex)) #ExEyEz+HxHyHz
    GaussInfo=np.loadtxt(GaussInfoFileName)
    for n, Info in enumerate(GaussInfo): # 遍历所有高斯波单元
        Farfield=Farfield+Info[5]*np.exp(1j*Info[6])*GaussianThetaPhi(k0,Info[0],Info[1],Info[2],Info[3],Info[4],Thetamesh,Phimesh,CalcFlag)
        print('\r', n, " of ", GaussInfo.shape[0],end='',flush=True)
    np.save(FarfieldFileName,Farfield) # (16900,6) joblib并行反效果
    return Farfield

def ToLocalCoordinate(WorldPxyz,LocalXV,LocalYV,LocalZV,LocalOriginAtW):
    V=np.array([LocalXV,LocalYV,LocalZV])
    V=np.linalg.inv(V)
    LocalPuvn=np.dot(WorldPxyz-LocalOriginAtW,V)
    return LocalPuvn

def ToWorldCoordinate(LocalPuvn,LocalXV,LocalYV,LocalZV,LocalOriginAtW):
    V=np.array([LocalXV,LocalYV,LocalZV])
    WorldPxyz=LocalOriginAtW+np.dot(LocalPuvn,V)
    return WorldPxyz


'''
Ver1.0
  1. 基于AdaptiveGB-VTK-Radome-7.0版，反射4.5版
  2. 各函数分别加入独立变量，如k0,CalcFlag
  3. 完全脱离vtk，用到的的坐标点序列，是numpy数组
  4. 去掉normalization？
  5. 中间想显示怎么办？
Ver2.0
  1. 加入曲面分解
  2. 

  
'''
