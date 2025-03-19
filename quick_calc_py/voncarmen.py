import sys
import numpy as np
import matplotlib.pyplot as plt
from scipy.interpolate import interp1d
# from initial_input import initData
sys.path.append(".")  # 把上级目录加入模块搜索

sampling=1500

#结合8院给的算例，馈源所在平面距离天线罩底面0.075m，生成的天线罩需要截断
for layer in range(1,4):
# layer=3 #当前生成第几层，最外层为0固定，内层为3
        vclinez=np.zeros(sampling)
        vclinex=np.zeros(sampling)
        start=0.0 + current/1000 #参数a
        bottom=0.567 #len
        step=(bottom-start)/sampling
        rr=0.1016-current/1000 #参数b
        threshold=rr/np.sqrt(np.arccos(-1))
        for ii in range(sampling):
            vclinez[ii]=start+step*ii
            theta=np.arccos(1-(2*step*ii/(bottom-start)))
            vclinex[ii]=threshold*np.sqrt(theta-0.5*np.sin(2*theta))

        tip_R=(10-0.5*current)/1000 #球头半径，毫米，参数r
        print('a=',start,'b=',rr,'r=',tip_R)
        np.savetxt(linepath+'a='+str(start)+'_b='+str(rr)+'_r='+str(tip_R)+'_'+str(layer)+'.txt',vclinex,fmt='%.5f')

        R_scan=int(sampling/10) #总长度的十分之一，求这一段每个离散点对应的球头半径和圆心
        scanz=vclinez[0:R_scan]
        scanx=vclinex[0:R_scan]
        grad=np.gradient(scanx,scanz) #每一点切线斜率
        scantip_o=grad*scanx+scanz #法线与横轴z交点
        scantip_R=np.sqrt(scanx**2+(scantip_o-scanz)**2) #截距，即球头半径
        ffR=interp1d(scantip_R,scantip_o) #插值搜索
        ffz=interp1d(scantip_R,scanz)
        tip_o=ffR(tip_R)
        tip_z=ffz(tip_R) #切点横坐标
        #画图，3/8圆
        tipx=np.arange(tip_o-tip_R,tip_o+0.5*tip_R,step)
        tipy=np.sqrt(tip_R**2-(tipx-tip_o)**2)

        # phi=np.arctan()

        plt.rcParams['figure.dpi'] = 80  # 分辨率
        font = {'family': 'serif',
                'serif': 'Times New Roman',
                'weight': 'normal',
                'size': 10}
        plt.rc('font', **font)
        # plt.rcParams['font.family']='Times New Roman ,SimSun'
        fig1 = plt.figure()
        # plt.plot(np.linspace(0, 30, 6), tbl0past, color='black', linewidth=1.5,label='追踪深度=6')
        # plt.plot(np.linspace(0, 30, 11), tbl90, color='black', linewidth=1.5,label='快速算法')
        plt.plot(vclinex*1000, (bottom-vclinez)*1000, color='black', linewidth=1.5)
        plt.plot(tipy*1000, (bottom-tipx)*1000, color='black', linewidth=1.5,linestyle='--')
        # plt.plot(tip_zz*1000, tip_xx*1000, color='black', linewidth=1.5,linestyle='--')
        # plt.plot(tip_oo*1000, tip_xx*1000, color='black', linewidth=1.5,linestyle='--')
        # plt.plot(scanz*1000, tip_Rscan*1000, color='black', linewidth=1.5,linestyle='--')

        # plt.plot(np.linspace(0, 30, 11), feko90, color='blue', linewidth=1.5,label='feko水平极化',linestyle='--')
        # plt.scatter(np.linspace(0, 30, 11),tbl90_8,c='blue',marker='x',label='feko')

        plt.xlabel('x (mm)',fontsize=14)
        plt.ylabel('z (mm)',fontsize=14)
        # plt.title('Transmittance Comparison')
        # plt.xticks(np.linspace(0, 12, 5))
        # plt.ylim(65, 105)
        # plt.legend()
        plt.grid()
        # plt.savefig('wx1_17G_tblh.tif')
        # plt.show()

        tipnum=int((bottom-tip_o+tip_R)/step)+1
        prt1=int((tip_z-(tip_o-tip_R))/step)+1
        prt2=tipnum-prt1
        points=np.zeros([tipnum,3])
        points[0:prt1,0]=tipy[0:prt1]
        points[prt1:,0]=vclinex[(sampling-tipnum+prt1):]
        points[0:prt1,2]=bottom-tipx[0:prt1]
        points[prt1:,2]=bottom-vclinez[(sampling-tipnum+prt1):]

        np.savetxt(linepath+str(current)+'_'+str(tip_R*1000)+'_'+str(layer)+'.txt',points,fmt='%.7f')

        fig2 = plt.figure(figsize=(6,12))
        plt.plot(points[:,0]*1000, points[:,2]*1000, color='black', linewidth=1.5)
        # plt.plot(tipy*1000, (bottom-tipx)*1000, color='black', linewidth=1.5,linestyle='--')

        plt.xlabel('x (mm)',fontsize=14)
        plt.ylabel('z (mm)',fontsize=14)
        plt.grid()
        # plt.show()


