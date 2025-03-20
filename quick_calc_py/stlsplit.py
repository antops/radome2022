import vtkmodules.all as vtk
from vtkmodules.util.numpy_support import vtk_to_numpy, numpy_to_vtk
import time
import sys
import numpy as np
import time
import os
import psutil
from initial_input import initData
sys.path.append(".")  # 把上级目录加入模块搜索

if getattr(sys, 'frozen', False):
    path = os.path.dirname(sys.executable)
elif __file__:
    path = os.path.dirname(__file__)

def show_info():
    pid=os.getpid()
    p=psutil.Process(pid)
    info=p.memory_full_info()
    memory=info.uss/1024/1024/1024
    return memory

version=1
jsopath = sys.argv[1]
# jsopath = 'stl_meta.json'
content = initData(jsopath)
if content["version"] != version:
    print('version error', flush=True)
    sys.exit()

numofstl=len(content['stlpath'])
stlpath_inner = content['stlpath'][numofstl-1]['readpath']
stlsavepath_inner = content['output'][numofstl]['writepath']+'.stl'

readerSTL = vtk.vtkSTLReader()
readerSTL.SetFileName(stlpath_inner)
GeomFilter = vtk.vtkTriangleFilter()
GeomFilter.SetInputConnection(readerSTL.GetOutputPort())
GeomFilter.Update()
normalFilter = vtk.vtkPolyDataNormals()  # 处理法向量
normalFilter.SetInputConnection(readerSTL.GetOutputPort())
normalFilter.SetComputePointNormals(1)
normalFilter.SetComputeCellNormals(1)
normalFilter.SetAutoOrientNormals(1)
normalFilter.Update()
arrNormals = vtk_to_numpy(normalFilter.GetOutput().GetCellData().GetNormals())
myscalars= numpy_to_vtk(arrNormals[:,2].ravel(), deep=True, array_type=vtk.VTK_DOUBLE)
normalFilter.GetOutput().GetCellData().SetScalars(myscalars)# //scalars设置更新完成
normalFilter.Update()
threshfilter = vtk.vtkThreshold()
threshfilter.SetInputConnection(normalFilter.GetOutputPort()) #过滤scalars（法向Z分量的值）
threshfilter.ThresholdBetween(-1+1e-3,0) #//默认用celldata的scalar判断??
threshfilter.Update()# // is of type vtkUnstructuredGrid, but a vtkPolyData is required
polydataInner = vtk.vtkGeometryFilter()
polydataInner.SetInputConnection(threshfilter.GetOutputPort())
polydataInner.Update()
mystl=vtk.vtkSTLWriter()
mystl.SetFileName(stlsavepath_inner)
mystl.SetInputConnection(polydataInner.GetOutputPort())
mystl.Write()

for nn in range(numofstl):
    stlpath = content['stlpath'][nn]['readpath']
    stlsavepath = content['output'][nn]['writepath']+'.stl'

    readerSTL = vtk.vtkSTLReader()
    readerSTL.SetFileName(stlpath)
    GeomFilter = vtk.vtkTriangleFilter()
    GeomFilter.SetInputConnection(readerSTL.GetOutputPort())
    GeomFilter.Update()
    normalFilter = vtk.vtkPolyDataNormals()  # 处理法向量
    normalFilter.SetInputConnection(readerSTL.GetOutputPort())
    normalFilter.SetComputePointNormals(1)
    normalFilter.SetComputeCellNormals(1)
    normalFilter.SetAutoOrientNormals(1)
    normalFilter.Update()
    arrNormals = vtk_to_numpy(normalFilter.GetOutput().GetCellData().GetNormals())
    myscalars = numpy_to_vtk(arrNormals[:, 2].ravel(), deep=True, array_type=vtk.VTK_DOUBLE)
    normalFilter.GetOutput().GetCellData().SetScalars(myscalars)  # //scalars设置更新完成
    normalFilter.Update()
    threshfilter = vtk.vtkThreshold()
    threshfilter.SetInputConnection(normalFilter.GetOutputPort())  # 过滤scalars（法向Z分量的值）
    threshfilter.SetLowerThreshold(0) # //默认用celldata的scalar判断??
    threshfilter.Update()
    polydataOuter = vtk.vtkGeometryFilter()
    polydataOuter.SetInputConnection(threshfilter.GetOutputPort())
    polydataOuter.Update()
    mystl = vtk.vtkSTLWriter()
    mystl.SetFileName(stlsavepath)
    mystl.SetInputConnection(polydataOuter.GetOutputPort())
    mystl.Write()








