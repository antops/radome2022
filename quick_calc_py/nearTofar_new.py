from numba import cuda
import numpy as np
@cuda.jit(device=True)
def cross(r1,r2):
    return r1[1]*r2[2]-r1[2]*r2[1],r1[2]*r2[0]-r1[0]*r2[2],r1[0]*r2[1]-r1[1]*r2[0]

@cuda.jit(device=True)
def dot(r1,r2):
    return r1[0]*r2[0]+r1[1]*r2[1]+r1[2]*r2[2]

@cuda.jit(device=True)
def vec_sub(r1,r2):
    return r1[0]-r2[0],r1[1]-r2[1],r1[2]-r2[2]

@cuda.jit(device=True)
def vec_add(r1,r2):
    return r1[0]+r2[0],r1[1]+r2[1],r1[2]+r2[2]

@cuda.jit(device=True)
def vec_dot(r1,r2):
    return r1[0]*r2[0]+r1[1]*r2[1]+r1[2]*r2[2]

@cuda.jit(device=True)
def c_dot_cet(c,r):
    return c*r[0],c*r[1],c*r[2]

import math
@cuda.jit(device=True)
def com_exp(angle):
    return math.cos(angle)+1j*math.sin(angle)
@cuda.jit
def gpu_PO(rs,r_cel,da,normal,Ei,Hi,numCell,c1,c2,kk,Es):
    theta_index = cuda.blockIdx.x
    phi_index = cuda.threadIdx.x
    for i in range(numCell):
        TTT = -da[i] * c1 * com_exp(kk * vec_dot(rs[theta_index, phi_index, :], r_cel[i]))
        kkk = cross(rs[theta_index, phi_index, :], vec_sub(cross(rs[theta_index, phi_index, :], cross(normal[i], Hi[i])),c_dot_cet((c2 / c1).real, cross(normal[i], Ei[i]))))
        Es[theta_index, phi_index, :] = vec_add(Es[theta_index, phi_index, :], c_dot_cet(TTT,kkk))

def calculate_FarField_nb(rs_theta,rs_phi,R_cell,Da,Normal,EXYZ,HXYZ,Freq):
    num_theta = len(rs_theta)
    num_phi = len(rs_phi)
    num_cell = len(Da)
    Threads_per_block = num_phi
    Block_per_gridx = int(np.ceil(num_theta*num_phi/ Threads_per_block))
    rs = np.zeros([num_theta,num_phi,3],dtype=np.float64)
    for i in range(num_theta):
        for j in range(num_phi):
            rs[i,j,:] = np.array([np.cos(rs_phi[j])*np.sin(rs_theta[i]),np.sin(rs_phi[j])*np.sin(rs_theta[i]),np.cos(rs_theta[i])])
    Es = np.zeros_like(rs,dtype=np.complex128)
    eps0 = 8.854e-12
    mu0 = 4 * np.pi * 1e-7
    c = 1 / np.sqrt(eps0 * mu0)
    Z0 = np.sqrt(mu0 / eps0)
    f = Freq
    lam = c / f
    k = 2 * np.pi / lam
    R = 900
    w = 2*np.pi*f
    Gr0 = np.exp(-1j*k*R)/4/np.pi/R
    const1 = -1j*w*mu0*Gr0
    const2 = -1j*k*Gr0
    rs_cuda = cuda.to_device(rs)
    r_cell_cuda = cuda.to_device(R_cell)
    Ei_cuda = cuda.to_device(EXYZ)
    Hi_cuda = cuda.to_device(HXYZ)
    normal_cuda = cuda.to_device(Normal)
    Da_cuda = cuda.to_device(Da)
    Es_cuda = cuda.to_device(Es)
    gpu_PO[Block_per_gridx, Threads_per_block](rs_cuda,r_cell_cuda,Da_cuda,normal_cuda,Ei_cuda,Hi_cuda,num_cell,const1,const2,k,Es_cuda)
    cuda.synchronize()
    Es = Es_cuda.copy_to_host()
    Es_ABS = np.zeros([num_theta,num_phi])
    for i in range(num_theta):
        for j in range(num_phi):
            Es_ABS[i,j] = np.linalg.norm(Es[i,j,:])
    return Es,Es_ABS


# #   远场角度
# theta = np.linspace(0, 20, num=101)
# theta = theta * np.pi/180.0
# phi = np.linspace(0, 360, num=181)
# phi = phi * np.pi / 180.0
#
# r_cell = np.load('r_cell.npy')
# da = np.load('da.npy')
# normal = np.load('normal.npy')
# E_xyz = np.load('E_XYZ.npy')
# H_xyz = np.load('H_XYZ.npy')
# f = 47e9
#
# import matplotlib.pyplot as plt
# Es,Es_abs = calculate_FarField_nb(theta,phi,r_cell,da,normal,E_xyz,H_xyz,f)
# plt.contourf(Es_abs)
# plt.show()

