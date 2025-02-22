# -*- coding: utf-8 -*-
"""
Created on Tue May 17 16:55:11 2022

@author: Administrator
"""
import math
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import pyplot as plt
from scipy.interpolate import griddata
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
#物理基础参数
f0=17e9
lamda=2.99792e8/f0
k0 = 2 * np.pi / lamda

FieldInput=np.load("ExEyHxHy0.7mm0.7mm17GHzSingle.npy")
FieldY=FieldInput[:,:,1]
Yabs=np.abs(FieldY)
Yabs=Yabs.reshape(-1,1)
Yang=np.angle(FieldY,deg=True)#############之前的复杂源转换为弧度制，师姐使用角度制，待验证
Yang=Yang.reshape(-1,1)
num=len(Yabs)
Xabs=np.zeros([num,1])
Xang=np.zeros([num,1])
fekofield=np.concatenate([Xabs,Xang,Yabs,Yang],axis=1)
np.savetxt('ExEyHxHy0.7mm0.7mm17GHzSingledeg.txt',fekofield,fmt="%.5f")
print(fekofield)