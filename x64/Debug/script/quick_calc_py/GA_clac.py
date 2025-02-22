import time

import matplotlib.pyplot as plt
import sys
import numpy as np
from sko.GA import GA
sys.path.append(".")  # 把上级目录加入模块搜索

import pandas as pd
import matplotlib.pyplot as plt
from initial_input import initData

version=1
jsopath = sys.argv[1]
content = initData(jsopath)
size_pop=content["size_pop"]
max_iter=content["max_iter"]

if content["version"] != version:
    print('version error', flush=True)
    sys.exit()

n_layer=content["n"]
para_l=np.zeros(n_layer*2)
para_u=np.zeros(n_layer*2)
flag=np.zeros(n_layer*2)
cnt_d=0
cnt_e=0
for ii in range(n_layer):
    para_l[ii*2] = content["model_meta"][ii]["para"]["d_min"]
    para_u[ii*2] = content["model_meta"][ii]["para"]["d_max"]
    if para_u[ii*2]!=para_l[ii*2]:
        flag[ii*2]=1
        cnt_d=cnt_d+1
    para_u[ii * 2]=para_u[ii*2]+1e-3
    para_l[ii*2+1] = content["model_meta"][ii]["para"]["e_min"]
    para_u[ii*2+1] = content["model_meta"][ii]["para"]["e_max"]
    if para_u[ii*2+1]!=para_l[ii*2+1]:
        flag[ii*2+1]=1
        cnt_e = cnt_e + 1
    para_u[ii * 2+1]=para_u[ii*2+1]+1e-3

def ff(x):
    size=len(x)
    ffy=0
    for nn in range(size):
        if flag[nn]==1:
            ffy=ffy+x[nn]*x[nn]
    return ffy

def demo_func(x):
    zzz=ff(x)
    global cnt
    cnt=cnt+1
    progress_cnt=cnt/(size_pop*max_iter)*100
    print('\r' + str(int(progress_cnt)), end='', flush=True)
    # zzz=x3*2+x4
    return zzz
cnt=0
ga = GA(func=demo_func, n_dim=n_layer*2,size_pop=size_pop,prob_mut=content["prob_mut"],lb=para_l, ub=para_u,max_iter=max_iter)
best_x, best_y = ga.run()

print('最佳参数组合: ',best_x,' 最大透波率:',100-best_y)

resultpath=content["result_path"]

FitV_history = pd.DataFrame(ga.all_history_FitV)
Fit_X=pd.DataFrame(ga.generation_best_X)
Fit_X_pltd=np.zeros([max_iter,cnt_d])
Fit_X_plte=np.zeros([max_iter,cnt_e])
cnt_d=0
cnt_e=0
for jj in range(2*n_layer):
    if jj%2==0 and flag[jj]==1:
        Fit_X_pltd[:, cnt_d] = Fit_X.values[:, jj]
        cnt_d=cnt_d+1
    if jj % 2 == 1 and flag[jj] == 1:
        Fit_X_plte[:, cnt_e] = Fit_X.values[:, jj]
        cnt_e=cnt_e+1
plt_max = FitV_history.max(axis=1)

plt.rcParams['font.family']='Times New Roman ,SimSun '
fig1=plt.figure()
plt.plot(FitV_history.index, FitV_history.values, '.', color='red')
plt.xlabel('迭代代数',fontsize=14)
plt.ylabel('适应度函数值',fontsize=14)
plt.title('每代所有个体适应度',fontsize=16)
plt.savefig(resultpath+'fitV.jpg')

fig2=plt.figure()
plt.plot(plt_max.index, plt_max, label='max')
plt.plot(plt_max.index, plt_max.cummax())
# plt.colorbar()
plt.xlabel('迭代代数',fontsize=14)
plt.ylabel('最佳个体适应度',fontsize=14)
plt.title('每代最佳个体适应度',fontsize=16)
plt.savefig(resultpath+'fitVmax.jpg')

fig3=plt.figure(figsize=(12,6))
plt.subplot(121)
plt.plot(Fit_X.index,Fit_X_pltd, marker='.', color='red',label='表层厚度')
# plt.legend()
plt.xlabel('迭代代数',fontsize=14)
plt.ylabel('优化层厚度',fontsize=14)
plt.subplot(122)
plt.plot(Fit_X.index,Fit_X_plte, marker='.', color='blue',label='表层介电常数')
# plt.legend()
plt.xlabel('迭代代数',fontsize=14)
plt.ylabel('优化层介电常数',fontsize=14)
plt.suptitle('夹层天线罩设计参数变化过程',fontsize=16)
# plt.title('夹层天线罩设计参数变化过程')
plt.savefig(resultpath+'para.jpg')
cnt=0
# plt.show()

np.savetxt(resultpath + '/results.txt', best_x)

#print('最佳参数组合: ',best_x,' 最大透波率:',100-best_y)

status_file = open(resultpath + '/status.txt','w')
#向文件中输入字符串
status_file.write('1\n')
status_file.close()
