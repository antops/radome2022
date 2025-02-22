# -*- coding: utf-8 -*-
"""
Created on Tue May 17 16:55:11 2022

@author: Administrator
"""
import numpy as np
from matplotlib import pyplot as plt
import json

def initData(jsopath):
    with open(jsopath,'r',encoding='utf-8') as file:
        content = json.load(file)
        return content

def initSource(sourcepath,polarization_mode):
    source=np.loadtxt(sourcepath,skiprows=1)
    ss=int(np.sqrt(len(source[:,2])))

    eps0 = 8.854e-12
    mu0 = 4 * np.pi * 1e-7
    c = 1 / np.sqrt(eps0 * mu0)
    Z0 = np.sqrt(mu0 / eps0)

    Yabs = source[:, 2]
    Yangle = source[:, 3] * np.pi / 180.
    Y = Yabs*np.exp(1j*Yangle)

    EY = Y.reshape(ss, ss,1)
    HX = 1 /(-Z0) * EY
    EX = np.zeros([ss, ss,1], dtype=complex)
    HY = np.zeros([ss, ss,1], dtype=complex)

    if polarization_mode==1:
        InputField=np.concatenate([EX,EY,HX,HY],axis=2)
    else:
        InputField=np.concatenate([EY,EX,HY,HX],axis=2)


    return InputField
