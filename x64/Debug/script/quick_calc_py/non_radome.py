import numpy as np
import matplotlib.pyplot as plt
from nearTofar_new import calculate_FarField_nb

def calculate_nonFarField_new(E_XYZ,H_XYZ, r_cell,da,normal,maxtheta,numtheta,maxphi,numphi,f0,outputpath):##normal默认为0,0,1
    """
        计算远场
    """
    #   远场角度
    theta = np.linspace(0, maxtheta, num=numtheta)
    theta = theta * np.pi / 180.0
    phi = np.linspace(0, maxphi, num=numphi)
    phi = phi * np.pi / 180.0

    Es, Es_abs = calculate_FarField_nb(theta, phi, r_cell, da, normal, E_XYZ, H_XYZ, f0)
    Es = Es.transpose(1, 0, 2)
    Es_abs = Es_abs.T
    np.save(outputpath + '/farField.npy', Es)
    np.save(outputpath + '/farFieldAbs.npy', Es_abs)

    # Num_theta, Num_phi = 101, 181  #
    thea_deg = np.linspace(0, maxtheta, numtheta)
    phi_deg = np.linspace(0, maxphi, numphi)
    thea_x = thea_deg * np.pi / 180
    phi_x = phi_deg * np.pi / 180
    [theaxx, phiyy] = np.meshgrid(thea_x, phi_x)
    xxp = np.cos(phiyy) * np.sin(theaxx)
    yyp = np.sin(phiyy) * np.sin(theaxx)

    dtheta = maxtheta / (numtheta - 1)
    dphi = maxphi / (numphi - 1)

    avePY = np.sum((pow(np.abs(Es[:, :, 0]), 2) + pow(np.abs(Es[:, :, 1]), 2) + pow(np.abs(Es[:, :, 2]), 2)) * np.sin(theaxx) * dtheta * dphi * np.pi * np.pi / (np.pi * 4 * 180 * 180))
    dbiPY = 10 * np.log10((pow(np.abs(Es[:, :, 0]), 2) + pow(np.abs(Es[:, :, 1]), 2) + pow(np.abs(Es[:, :, 2]), 2)) / avePY)
    dbiPY = dbiPY - 2  ###tbl_constant=2?

    plt.figure()
    plt.pcolormesh(xxp, yyp, dbiPY, cmap='jet', shading='gouraud')
    plt.colorbar()
    plt.title('Gain of non_radome', fontsize=14)
    plt.xlabel('u', fontsize=12);plt.ylabel('v', fontsize=12)
    plt.savefig(outputpath + '/farField.jpg')
    # plt.show()

