# -*- coding: utf-8 -*-
"""
Created on Thu Mar 23 14:03:08 2023

@author: quiet
"""

import time
import sys
import numpy as np
import matplotlib.pyplot as plt
import math 
import json
from scipy.special import jv
from initial_input import initData


jsopath = sys.argv[1]
content = initData(jsopath)
if content["version"] != 1 :
    print('version error',flush=True)
    sys.exit()

#用户可修改的参数，共7个
angle_theta = 35 #波束指向俯仰角
angle_phi = 45    #波束指向方位角
f0=17e9   #工作频率
Radius=75e-3    #口径场半径
dx = 0.5    #x方向上阵元间距
dy = 0.5    #y方向上阵元间距2
sll = 30    #旁瓣电平

angle_theta=content["theta"]
angle_phi = content["phi"]
f0 = content["fre"]
Radius = content["radius"]
dx = content["dx"]
dy = content["dy"]
sll = content["s11"]
result_path = content["result_path"]
print('theta:',angle_theta,',phi:',angle_phi,
    ',fre:',f0,',Radius:',Radius,',dx:',dx,',dy:',dy,
    ',sll:',sll, ',result_path:',result_path)


lamda=2.99792e8/f0
k0 = 2 * np.pi / lamda
ds = 1e-3 #网格大小
N = 161 #网格数

w0 = 0.27*lamda  #高斯波束束腰0.27

z = 0.0000000001

Wz = w0*np.sqrt(1+((lamda*z)/(np.pi*pow(w0,2)))**2)

Rz = z+(np.pi*pow(w0,2)/lamda)**2/z

angz = math.atan((lamda*z)/(np.pi*pow(w0,2)))

j, i=np.mgrid[0:N:1,0:N:1]
# X_Ey = (i-(N-1)/2)*ds-0.5*ds
X_Ey = (i-(N-1)/2)*ds#口面XY的坐标
Y_Ey = (j-(N-1)/2)*ds#口面XY的坐标
Xi = (i-(N-1)/2)*ds
Yi = (j-(N-1)/2)*ds

Ey_sum = np.zeros((N,N),dtype=complex)

def gauss1(x,y):    #对每个阵元，用一个高斯波束模拟
    rxy = np.sqrt(x**2+y**2)
    gauss = np.sqrt(2/(np.pi*pow(Wz,2)))*np.exp((-1*pow(rxy,2)/pow(Wz,2))-1j*(k0*z+(np.pi*pow(rxy,2))/(lamda*Rz))+1j*angz)
    return gauss

## .................................................
## 泰勒综合口径场
#.................................................

a = np.round(Radius/lamda)
mm = int(a/dx)
nn = int(a/dy)
R = pow(10,(sll/20))
A = math.acosh(R)/np.pi

n1 = 6 #泰勒分布的参数
xa = np.arange(0.1,100,0.0001)
ya = jv(1,xa);
nxa = np.size(xa,0)



#.................................................
## 基础设置完毕
#.................................................
count = 0
rn = np.zeros((0))
for ii in np.arange(0,nxa-1):
    if (ya[ii]*ya[ii+1]<=0):
        # rn[count] = (xa[ii]+xa[ii+1])/2/np.pi
        rn = np.insert(rn,0,((xa[ii]+xa[ii+1])/2/np.pi))
        count = count+1
rn = np.flip(rn)
rn1 = rn[n1-1]

dun = np.zeros((n1-1))
for ii in np.arange(0,n1-1):
    dun[ii] = rn1*np.sqrt(pow(A,2)+pow((ii+0.5),2))/np.sqrt(pow(A,2)+pow((n1-0.5),2))


Bm = np.zeros((n1-1))
for ii in np.arange(0,n1-1):
    Bm[ii] = -1/jv(0,np.pi*rn[ii])
    for jj in np.arange(0,n1-1):
        Bm[ii] = Bm[ii]*(1-pow(rn[ii],2)/pow(dun[jj],2))
        if (ii != jj):
            Bm[ii] = Bm[ii]/(1-pow(rn[ii],2)/pow(rn[jj],2))


posx0 = np.zeros((2*nn,2*mm)); posy0 = np.zeros((2*nn,2*mm)); mag0 = np.zeros((2*nn,2*mm));

for ii in np.arange(0,2*mm):
    for jj in np.arange(0,2*nn):

        x0 = (-mm+ii+0.5)*dx
        y0 = (nn-jj-0.5)*dy
        r0 = np.sqrt(pow(x0,2)+pow(y0,2))
        if r0<=a:
            posx0[jj,ii] = x0
            posy0[jj,ii] = y0
            p0 = np.pi*r0/a
            mag0[jj,ii] = 1
            for kk in np.arange(0,n1-1):
                mag0[jj,ii] = mag0[jj,ii]+Bm[kk]*jv(0,rn[kk]*p0)

posx = posx0*lamda
posy = posy0*lamda
mag0 = mag0/np.max(mag0[:,mm])  #获得按泰勒权重分布的阵元幅度
#计算相位，使波束指向theta,phi
exphase = np.sin(np.pi*angle_theta/180)*np.sin(np.pi*angle_phi/180)*posx*2*np.pi/lamda+np.sin(np.pi*angle_theta/180)*np.cos(np.pi*angle_phi/180)*posy*2*np.pi/lamda

## 生成高斯的圆阵列
Ey_sum = np.zeros((N,N),dtype=complex)
Gauss_Info = np.zeros((0, 7));GaussInfoFileName = ".\\GaussInfo"+str(int(f0/1e9))+".txt"

for ii in np.arange(0,2*mm):
    for jj in np.arange(0,2*nn):
        if mag0[ii,jj]!=0:
              yuan_gauss = gauss1(X_Ey+posx[ii,jj],Y_Ey+posy[ii,jj])
              Ey1 = mag0[ii,jj]*yuan_gauss*np.exp(1j*exphase[ii,jj])
              Ey_sum = Ey_sum+Ey1

tojson=np.zeros((N*N,4),dtype=float)
tojson[:,2]=np.abs(Ey_sum.T).reshape(N*N)
tojson[:,3]=(np.angle(Ey_sum.T)/np.pi*180).reshape(N*N)
np.savetxt(result_path + '/source.txt',tojson,fmt ='%.5f' )

plt.rcParams['figure.dpi'] = 80#分辨率
font = {'family': 'serif',
        'serif': 'Times New Roman',
        'weight': 'normal',
        'size': 10}
plt.rc('font', **font)
Ey_sum = Ey_sum
plt.figure(figsize=(15,6))
plt.subplot(121)
plt.pcolor(X_Ey,Y_Ey,np.abs(Ey_sum),cmap ='jet' ,shading='auto');
plt.colorbar()
plt.subplot(122)
plt.pcolor(X_Ey,Y_Ey,np.angle(Ey_sum),cmap ='jet' ,shading='auto');
plt.colorbar()
# plt.show()

# near = np.zeros((N,N,4),dtype=complex)
# near[:,:,1] = Ey_sum
# near[:,:,2] = Ey_sum/(-120*np.pi)
# np.save('.\\Efield'+str(int(f0/1e9))+'angle'+str(int(angle_theta))+'.npy',near)
status_file = open(result_path + '/source_status.txt','w')
#向文件中输入字符串
status_file.write('1\n')
status_file.close()