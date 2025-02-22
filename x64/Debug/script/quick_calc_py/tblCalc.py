import sys
import numpy as np
import os
from initial_input import initData
sys.path.append(".")  # 把上级目录加入模块搜索

if getattr(sys, 'frozen', False):
    path = os.path.dirname(sys.executable)
elif __file__:
    path = os.path.dirname(__file__)

#调用初始化接口文件
version = 1
# print(sys.argv)
jsopath = sys.argv[1]
# jsopath='meta.json'
syspath=os.path.dirname(os.path.realpath(sys.argv[0]))
content = initData(jsopath)
if content["version"] != version :
    print('version error',flush=True)
    sys.exit()

outputpath=content["output"]["result_path"]
radome = np.load(outputpath + '/farFieldAbs.npy')
ra_max = np.amax(radome)
outputpath_non = content["output"]["nonradome_path"]
non = np.load(outputpath_non + '/farFieldAbs.npy')
non_max = np.amax(non)
tbl = (ra_max / non_max) * 100
np.save(outputpath + '/tbl.npy', tbl)
print('透波率：', tbl, '%',flush=True)

status_file = open(outputpath + '/tbl_status.txt','w')
#向文件中输入字符串
status_file.write('1\n')
status_file.close()

