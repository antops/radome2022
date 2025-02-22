# -*- coding: utf-8 -*-
"""
Created on Thu Mar 23 14:03:08 2023

@author: quiet
"""

import time
import sys
import os
import numpy as np
import matplotlib.pyplot as plt
import math 
import json
from scipy.special import jv
from initial_input import initData

sys.path.append(".")  # 把上级目录加入模块搜索

if getattr(sys, 'frozen', False):
    path = os.path.dirname(sys.executable)
elif __file__:
    path = os.path.dirname(__file__)

version=1
jsopath = sys.argv[1]
#jsopath = 'source_meta.json'
content = initData(jsopath)
if content["version"] != version:
    print('version error', flush=True)
    sys.exit()
angle_theta = content["theta"]
angle_phi = content["phi"]
f0 = content["fre"]
Radius = content["radius"]
sll = content["s11"]
result_path = content["result_path"]
ds = content["ds"]
N = content["mesh_n"]
# print('theta:', angle_theta, ',phi:', angle_phi,
#       ',fre:', f0, ',Radius:', Radius, ',dx:', dx, ',dy:', dy,
#       ',sll:', sll, ',result_path:', result_path)

lamda = 2.99792e8 / f0
k0 = 2 * np.pi / lamda

j, i = np.mgrid[0:N:1, 0:N:1]
# X_Ey = (i-(N-1)/2)*ds-0.5*ds
X_Ey = (i - (N - 1) / 2) * ds  # 口面XY的坐标
Y_Ey = (j - (N - 1) / 2) * ds  # 口面XY的坐标
Xi = (i - (N - 1) / 2) * ds
Yi = (j - (N - 1) / 2) * ds

Ey_sum = np.zeros((N, N), dtype=complex)

## 泰勒综合口径场
# .................................................
# lamdatest = 2.99792e8 / 15e9
a = Radius
mm = int(a / ds)
nn = int(a / ds)
R = pow(10, (sll / 20))
A = math.acosh(R) / np.pi

n1 = 6  # 泰勒分布的参数
xa = np.arange(0.1, 100, 0.0001)
ya = jv(1, xa);
nxa = np.size(xa, 0)

# .................................................
## 基础设置完毕
# .................................................
count = 0
rn = np.zeros((0))
for ii in np.arange(0, nxa - 1):
    if (ya[ii] * ya[ii + 1] <= 0):
        # rn[count] = (xa[ii]+xa[ii+1])/2/np.pi
        rn = np.insert(rn, 0, ((xa[ii] + xa[ii + 1]) / 2 / np.pi))
        count = count + 1
rn = np.flip(rn)
rn1 = rn[n1 - 1]

dun = np.zeros((n1 - 1))
for ii in np.arange(0, n1 - 1):
    dun[ii] = rn1 * np.sqrt(pow(A, 2) + pow((ii + 0.5), 2)) / np.sqrt(pow(A, 2) + pow((n1 - 0.5), 2))

Bm = np.zeros((n1 - 1))
for ii in np.arange(0, n1 - 1):
    Bm[ii] = -1 / jv(0, np.pi * rn[ii])
    for jj in np.arange(0, n1 - 1):
        Bm[ii] = Bm[ii] * (1 - pow(rn[ii], 2) / pow(dun[jj], 2))
        if (ii != jj):
            Bm[ii] = Bm[ii] / (1 - pow(rn[ii], 2) / pow(rn[jj], 2))
xx=int((N-1)/2)
posx0 = np.zeros((2 * xx+1, 2 * xx+1));
posy0 = np.zeros((2 * xx+1, 2 * xx+1));
mag0 = np.zeros((2 * xx+1, 2 * xx+1));

for ii in np.arange(0, 2 * xx+1):
    for jj in np.arange(0, 2 * xx+1):

        x0 = (-xx + ii + 0.5) * ds
        y0 = (xx - jj - 0.5) * ds
        r0 = np.sqrt(pow(x0, 2) + pow(y0, 2))
        if r0 <= a:
            posx0[jj, ii] = x0
            posy0[jj, ii] = y0
            p0 = np.pi * r0 / a
            mag0[jj, ii] = 1
            for kk in np.arange(0, n1 - 1):
                mag0[jj, ii] = mag0[jj, ii] + Bm[kk] * jv(0, rn[kk] * p0)

posx = posx0 * lamda
posy = posy0 * lamda
mag0 = mag0 / np.max(mag0[:, xx])  # 获得按泰勒权重分布的阵元幅度
# 计算相位，使波束指向theta,phi
exphase = np.sin(np.pi * angle_theta / 180) * np.sin(
    np.pi * angle_phi / 180) * posx * 2 * np.pi / lamda + np.sin(np.pi * angle_theta / 180) * np.cos(
    np.pi * angle_phi / 180) * posy * 2 * np.pi / lamda
# Ey_sum = np.zeros((N, N), dtype=complex)

Ey_sum=mag0 * np.exp(1j * exphase)
nnn=np.angle(Ey_sum)
tojson = np.zeros((N * N, 4), dtype=float)
tojson[:, 2] = np.abs(Ey_sum).reshape(N * N)
tojson[:, 3] = (np.angle(Ey_sum) / np.pi * 180).reshape(N * N)
np.savetxt(result_path + '/source.txt', tojson, fmt='%.5f')

#line = 'N='+str(N) + ' ' + str(N) + ' ds=' + str(ds)+ ' th=' + str(angle_theta)+ ' ph=' + str(angle_phi)+ ' f0=' + str(f0/1e9)
line = str(N)+' '+str(N)+' '+' '+str(ds)
with open(result_path + '/source.txt', 'r+') as f:
    content = f.read()
    f.seek(0, 0)
    f.write(line.rstrip('\r\n') + '\n' + content)

# plt.rcParams['figure.dpi'] = 330  # 分辨率
# font = {'family': 'serif',
#         'serif': 'Times New Roman',
#         'weight': 'normal',
#         'size': 10}
# plt.rc('font', **font)
# fig1=plt.figure()
# plt.pcolor(X_Ey, Y_Ey, np.abs(Ey_sum), cmap='jet', shading='auto');
# plt.xticks([])
# plt.yticks([])
# # plt.colorbar()
# plt.savefig(result_path + '/source.jpg')
# fig2=plt.figure()
# plt.pcolor(X_Ey, Y_Ey, np.angle(Ey_sum), cmap='jet', shading='auto');
# plt.xticks([])
# plt.yticks([])
# # plt.colorbar()
# plt.savefig(result_path + '/sourcephase.jpg')

# near = np.zeros((N,N,4),dtype=complex)
# near[:,:,1] = Ey_sum
# near[:,:,2] = Ey_sum/(-120*np.pi)
# np.save('.\\Efield'+str(int(f0/1e9))+'angle'+str(int(angle_theta))+'.npy',near)
status_file = open(result_path + '/source_status.txt', 'w')
# 向文件中输入字符串
status_file.write('1\n')
status_file.close()

