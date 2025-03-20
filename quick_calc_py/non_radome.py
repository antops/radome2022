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
    np.save(outputpath + '/farFieldAbs.npy', Es_abs/pow(1.259,0.5))
    np.savetxt(outputpath + '/farFieldAbs.txt', Es_abs / pow(1.259, 0.5))

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
    dbiPY = dbiPY - 1
    ###tbl_constant=2?
    np.savetxt(outputpath + '/farFieldAbsdBi.txt', dbiPY)

    fig1 = plt.figure()
    plt.pcolormesh(xxp, yyp, dbiPY, cmap='jet', shading='gouraud')
    plt.colorbar()
    plt.title('Gain of non_radome', fontsize=14)
    plt.xlabel('u', fontsize=12);plt.ylabel('v', fontsize=12)
    # plt.savefig(outputpath + '/farField.jpg')
    # plt.show()

    MAX = np.max(dbiPY) + 1
    angle_phi=0
    bb1 = np.flip(dbiPY[int(angle_phi / 2 + 90), 1:numtheta])
    bb2 = dbiPY[int(angle_phi / 2), 0:numtheta]
    bb_E_dbi = np.concatenate((bb1, bb2), axis=0)
    fig2 = plt.figure()
    plt.plot(np.linspace(0 - maxtheta, maxtheta, numtheta * 2 - 1), bb_E_dbi, color='blue', linewidth=2,
             linestyle='dashed')
    plt.title('E-plane Gain', fontsize=14)
    plt.xlabel('Theta(deg)');
    plt.ylabel('Mag(dB)')
    plt.xticks(np.linspace(0 - maxtheta, maxtheta, 11))
    plt.xlim(0 - maxtheta, maxtheta)
    plt.ylim(MAX - 35, MAX)
    plt.grid()
    # plt.savefig(outputpath + '/nonFarFieldEplane.jpg')
    maxE = np.argmax(bb_E_dbi)
    maxEx = np.linspace(0 - maxtheta, maxtheta, numtheta * 2 - 1)[maxE]

    angle_phi = 90
    bb1 = np.flip(dbiPY[int(angle_phi / 2 + 90), 1:numtheta])
    bb2 = dbiPY[int(angle_phi / 2), 0:numtheta]
    bb_H_dbi = np.concatenate((bb1, bb2), axis=0)
    fig3 = plt.figure()
    plt.plot(np.linspace(0 - maxtheta, maxtheta, numtheta * 2 - 1), bb_H_dbi, color='blue', linewidth=2,
             linestyle='dashed')
    plt.title('H-plane Gain', fontsize=14)
    plt.xlabel('Theta(deg)');
    plt.ylabel('Mag(dB)')
    plt.xticks(np.linspace(0 - maxtheta, maxtheta, 11))
    plt.xlim(0 - maxtheta, maxtheta)
    plt.ylim(MAX - 35, MAX)
    plt.grid()
    # plt.savefig(outputpath + '/nonFarFieldHplane.jpg')
    # plt.show()
    maxH = np.argmax(bb_H_dbi)
    maxHx = np.linspace(0 - maxtheta, maxtheta, numtheta * 2 - 1)[maxH]
    # print("non_maxEx=", maxEx, "non_maxHx=", maxHx)

def calculate_nonFarField(FieldInput,ds,f0,maxtheta,numtheta,maxphi,numphi,outputpath):##normal默认为0,0,1
    near = FieldInput

    #   面位置
    N = int(np.size(near, 0))
    numOfCell = N * N
    r_cell = np.zeros((numOfCell, 3))
    normal = np.array([0, 0, 1]) * np.ones((numOfCell, 3))
    da = ds * ds * np.ones(numOfCell)

    #   面上的场
    E_XYZ = np.zeros((numOfCell, 3), dtype=complex)
    H_XYZ = np.zeros((numOfCell, 3), dtype=complex)

    for i in range(N):
        for j in range(N):
            r_cell[i * N + j, :] = np.array([(i - (N - 1) / 2) * ds, (j - (N - 1) / 2) * ds, 5.03697009E-02])
            E_XYZ[i * N + j, :] = np.array([near[i, j, 0], near[i, j, 1], 0])
            H_XYZ[i * N + j, :] = np.array([near[i, j, 2], near[i, j, 3], 0])

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

    avePY = np.sum((pow(np.abs(Es[:, :, 0]), 2) + pow(np.abs(Es[:, :, 1]), 2) + pow(np.abs(Es[:, :, 2]), 2)) * np.sin(
        theaxx) * dtheta * dphi * np.pi * np.pi / (np.pi * 4 * 180 * 180))
    dbiPY = 10 * np.log10(
        (pow(np.abs(Es[:, :, 0]), 2) + pow(np.abs(Es[:, :, 1]), 2) + pow(np.abs(Es[:, :, 2]), 2)) / avePY)
    dbiPY = dbiPY - 1  ###tbl_constant=2?
    np.savetxt(outputpath + '/farFieldAbsdBi.txt', dbiPY)


    plt.figure()
    plt.pcolormesh(xxp, yyp, dbiPY, cmap='jet', shading='gouraud')
    plt.colorbar()
    plt.title('Gain of non_radome', fontsize=14)
    plt.xlabel('u', fontsize=12);
    plt.ylabel('v', fontsize=12)
    plt.savefig(outputpath + '/farField.jpg')

    # plt.figure()
    # plt.pcolormesh(xxp, yyp, np.abs(Es[:, :, 1]), cmap='jet', shading='gouraud')
    # plt.colorbar()
    # plt.savefig(outputpath + '/non_FarFielddisk.jpg')
    # plt.show()