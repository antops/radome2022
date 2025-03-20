# -*- coding: utf-8 -*-
"""
Created on Tue Apr 12 12:34:50 2022
https://blog.csdn.net/weixin_40744368/article/details/102740932
@author: baiming
Version 2.0
vtk显示函数集
"""
import vtk
import numpy as np
import matplotlib.pyplot as plt
from vtk.util.numpy_support import numpy_to_vtk

lookupTable=vtk.vtkLookupTable()
lookupTable.Build() #默认256个颜色,先build再改
for i in np.arange(0,lookupTable.GetNumberOfColors()):
    rgba=lookupTable.GetTableValue(i)
    lookupTable.SetTableValue(i,rgba[2],rgba[1],rgba[0],rgba[3]) #彩虹映射表
    
#-------二维或三维平面图-------------
def PointsActor2D(FieldSource,PtsCoordsWorld): # (取layers=0, X, Y, 6) 
    FieldDraw=np.abs(FieldSource)  # Ex 或 Ey 或 Ez
    polydataLocal=vtk.vtkPolyData()
    pointsLocal=vtk.vtkPoints()
    vtk_Points=numpy_to_vtk(PtsCoordsWorld, deep=True, array_type=vtk.VTK_DOUBLE)
    pointsLocal.SetData(vtk_Points)
    polydataLocal.SetPoints(pointsLocal)#此处塞进来的vtk_Points是一个在世界坐标系下的点
    vtk_FieldDraw = numpy_to_vtk(FieldDraw.ravel(), deep=True, array_type=vtk.VTK_DOUBLE)
    polydataLocal.GetPointData().SetScalars(vtk_FieldDraw) #场值数据赋值给points的scalars去显示
    # 顶点相关的 filter
    vertex = vtk.vtkVertexGlyphFilter()
    vertex.SetInputData(polydataLocal)
    polydataWorldmapper = vtk.vtkPolyDataMapper()
    polydataWorldmapper.SetInputConnection(vertex.GetOutputPort()) # 内壁的数据结构
    polydataWorldmapper.SetScalarRange(0,np.max(FieldDraw)) #lookuptable的range设置无效
    polydataWorldmapper.SetLookupTable(lookupTable)
    polydataWorldmapper.Update()
    world2DActor=vtk.vtkActor()
    world2DActor.SetMapper(polydataWorldmapper)
    return world2DActor

def polydataReflectorActor(FieldSource,polydataReflector): # (取layers=0, X, Y, 6) 
    FieldDraw=np.abs(FieldSource) # Ex 或 Ey 或 Ez
    # FieldDraw = np.angle(FieldSource)
    vtk_FieldDraw = numpy_to_vtk(FieldDraw.ravel(), deep=True, array_type=vtk.VTK_DOUBLE)
    polydataReflector.GetOutput().GetPointData().SetScalars(vtk_FieldDraw) #场值数据赋值给points的scalars去显示
    # vtk_FieldVector = numpy_to_vtk(np.real(FieldExEyEz), deep=True, array_type=vtk.VTK_DOUBLE)    
    # polydataReflector.GetPointData().SetVectors(vtk_FieldVector) #场值数据赋值给points的Vectors去显示
    
    #--------强度放在scalars----------------
    polydataReflectormapper = vtk.vtkPolyDataMapper()
    polydataReflectormapper.SetInputConnection(polydataReflector.GetOutputPort()) # 内壁的数据结构
    polydataReflectormapper.SetScalarRange(0,np.max(FieldDraw)) #lookuptable的range设置无效
    polydataReflectormapper.SetLookupTable(lookupTable)
    polydataReflectormapper.Update()
    STLReflectorActor=vtk.vtkActor()
    STLReflectorActor.SetMapper(polydataReflectormapper)
    #STLReflectorActor.GetProperty().SetOpacity(0.99)
    return STLReflectorActor


def polydataReflectorVectorActor(VectorSource,polydataReflector): # (取layers=0, X, Y, 6) 

    PoyntingVector=np.load(VectorSource)
    #----------电场或波印廷幅度值放在scalars去显示强度分布,Vertors只体现方向------
    #----------全矢量的real值放在Vectors去显示箭头方向，大小由Scalars/vector决定
    vtk_Poynting = numpy_to_vtk(PoyntingVector, deep=True, array_type=vtk.VTK_DOUBLE)
    polydataReflector.GetOutput().GetPointData().SetVectors(vtk_Poynting) #场值数据赋值给points的Vectors去显示
    
    #--------矢量放在vector里，由glyph1显示------
    mask1=vtk.vtkMaskPoints()
    mask1.SetInputConnection(polydataReflector.GetOutputPort())
    mask1.SetMaximumNumberOfPoints(10000)
    mask1.RandomModeOn()
    arrow1=vtk.vtkArrowSource()
    glyph1=vtk.vtkGlyph3D()
    glyph1.SetInputConnection(mask1.GetOutputPort())
    glyph1.SetSourceConnection(arrow1.GetOutputPort())
    glyph1.SetVectorModeToUseVector()  #方向用Vector 还是用 Normal
    glyph1.SetScaleModeToScaleByVector() #大小用Vector 还是用 Scalars
#    glyph1.SetScaleModeToScaleByScalar() #大小用Vector 还是用 Scalars
#    glyph1.SetColorModeToColorByScalar() #颜色用vector还是用 Scalars强度
    glyph1.SetColorModeToColorByScalar() #颜色用vector还是用 Scalars强度
    glyph1.SetScaleFactor(0.03)
    glyphMapper1=vtk.vtkPolyDataMapper()
    glyphMapper1.SetInputConnection(glyph1.GetOutputPort())
    glyphMapper1.SetLookupTable(lookupTable)
    glyphMapper1.Update()
    glyphActor1=vtk.vtkActor()
    glyphActor1.SetMapper(glyphMapper1)
    glyphActor1.GetProperty().SetColor(0,1,1) #法向量Glyph影响后续的SetScalar值有冲突？？
    glyphActor1.GetProperty().SetOpacity(0.8)
    return glyphActor1


def LineActor(OriginPt,EndPt):
    lineSource= vtk.vtkLineSource()
    lineSource.SetPoint1(OriginPt);
    lineSource.SetPoint2(EndPt);
    lineSource.Update()
    lineMapper=vtk.vtkPolyDataMapper()
    lineMapper.SetInputConnection(lineSource.GetOutputPort())
    lineMapper.Update()
    LineActor=vtk.vtkActor()
    LineActor.SetMapper(lineMapper)
    return LineActor

#-------方向图立体显示-------------
# 方向图数据，角度本地点阵，本地点阵所在的坐标系轴，原点偏移显示
def PatternWorldActor(Farfield0,Thetamesh,Phimesh,FitAxisU,FitAxisV,FitAxisN,FitOrigin,OffsetOrigin):
    # Farfield=np.load(PatternFileName) #".\\Directivity.npy"
    PattDraw=np.abs(Farfield0[1,:,:]) #+Farfield1[1,:,:]+Farfield2[1,:,:]) # Ex Ey Etheta Ephi
    PhiNum = PattDraw.shape[1]
    vtk_PattDraw=numpy_to_vtk(PattDraw.ravel(), deep=True, array_type=vtk.VTK_DOUBLE)

    r=PattDraw/np.max(PattDraw) # 先固定球面 r=1，为了delauney2D 架设表面
    r=np.abs(np.log(r+1e-8)+8)/5
    PattX = r * np.sin(Thetamesh) * np.cos(Phimesh)
    PattY = r * np.sin(Thetamesh) * np.sin(Phimesh)
    PattZ = r * np.cos(Thetamesh)
    PattPts=np.stack((PattX, PattY,PattZ),axis=2).reshape(-1,3)
    PatternCoordsWorld=ToWorldCoordinate(PattPts,FitAxisU,FitAxisV,FitAxisN,FitOrigin) #导入的方向图是在曲面坐标系下计算的
    #转换为世界坐标系去显示，原点在曲面原点
    PatternCoordsWorld=PatternCoordsWorld-FitOrigin+OffsetOrigin #挪到出射面去显示
    
    PattPoints=vtk.vtkPoints()
    vtk_Points=numpy_to_vtk(PatternCoordsWorld, deep=True, array_type=vtk.VTK_DOUBLE)
    PattPoints.SetData(vtk_Points)
    
    triangle = vtk.vtkTriangle()
    cells =vtk.vtkCellArray()
    for i in np.arange(PhiNum , Thetamesh.size):
        if(i%PhiNum == 0):
            triangle.GetPointIds().SetId(0, i) #; //根据3个点定义三角形
            triangle.GetPointIds().SetId(1, i + PhiNum - 1) #;  //和前面一个相邻点
            triangle.GetPointIds().SetId(2, i - 1) #;  //以及它的上面一层的对应点
            cells.InsertNextCell(triangle) # ;  
            triangle.GetPointIds().SetId(0, i) # ; //根据3个点定义三角形
            triangle.GetPointIds().SetId(1, i - 1) #; //和它的上面一层的对应点
            triangle.GetPointIds().SetId(2, i - PhiNum) #; //和前面一个相邻点
            cells.InsertNextCell(triangle) #;   
        else:
            triangle.GetPointIds().SetId(0, i) #; //根据3个点定义三角形
            triangle.GetPointIds().SetId(1, i - 1) #;  //和前面一个相邻点
            triangle.GetPointIds().SetId(2, i - 1 - PhiNum) #;  //以及它的上面一层的对应点
            cells.InsertNextCell(triangle) # ;  
            triangle.GetPointIds().SetId(0, i) # ; //根据3个点定义三角形
            triangle.GetPointIds().SetId(1, i - PhiNum - 1) #; //和它的上面一层的对应点
            triangle.GetPointIds().SetId(2, i - PhiNum) #; //和前面一个相邻点
            cells.InsertNextCell(triangle) #;   
        
    Pattpolydata = vtk.vtkPolyData()
    Pattpolydata.SetPoints(PattPoints)
    Pattpolydata.SetPolys(cells)  #;   //拓扑单元集合
    Pattpolydata.GetPointData().SetScalars(vtk_PattDraw)

    Pattmapper = vtk.vtkPolyDataMapper()
    Pattmapper.SetInputData(Pattpolydata)
    Pattmapper.SetScalarRange(0, np.max(PattDraw))
    Pattmapper.SetLookupTable(lookupTable)
    PattActor = vtk.vtkActor()
    PattActor.SetMapper(Pattmapper)
    return PattActor

def AxesActor():
    transform =vtk.vtkTransform()
    transform.Translate(0.0, 0.0, 0.0)
    axesActor =vtk.vtkAxesActor()
    axesActor.SetPosition(0, 0, 0)
    axesActor.SetTotalLength(0.1, 0.1, 0.1)
    axesActor.SetShaftType(0)
    axesActor.SetAxisLabels(1)
    xAxisCaptionActor = axesActor.GetXAxisCaptionActor2D()
    yAxisCaptionActor = axesActor.GetYAxisCaptionActor2D()
    zAxisCaptionActor = axesActor.GetZAxisCaptionActor2D()
    xAxisCaptionActor.SetPosition2( 0.1, 0.0 )
    yAxisCaptionActor.SetPosition2( 0.1, 0.0 )
    zAxisCaptionActor.SetPosition2( 0.1, 0.0 )
    axesActor.SetCylinderRadius(0.01)
    axesActor.SetUserTransform(transform)
    return axesActor

def DrawALLActors(ActorCollections): 
    actornum = ActorCollections.GetNumberOfItems()
    ActorCollections.InitTraversal()

    ren1 = vtk.vtkRenderer()
    ren1.SetBackground(1.0, 1.0, 1.0)
    ren1.SetBackground2(0.1, 0.2, 0.4)
    ren1.SetGradientBackground(1)
    
    ren1.AddActor(AxesActor())

    for i in np.arange(1,actornum): 
    	ren1.AddActor(ActorCollections.GetNextActor())  #?????少加一个actor???

    myCam=vtk.vtkCamera()
    myCam.SetPosition(0,5,0.6)
    myCam.SetFocalPoint(0,0,1.0)
#    myCam.ComputeViewPlaneNormal() #//自动
    ren1.SetActiveCamera(myCam)

    renWin = vtk.vtkRenderWindow()
    renWin.AddRenderer(ren1)
    renWin.SetSize(800, 600)
    renWin.Render()
    
    iren = vtk.vtkRenderWindowInteractor()
    iren.SetRenderWindow(renWin)
    irenStyle =vtk.vtkInteractorStyleTrackballCamera()
    iren.SetInteractorStyle(irenStyle)
    iren.Initialize()
    iren.Start()
    return

def ToLocalCoordinate(WorldPxyz,LocalXV,LocalYV,LocalZV,LocalOriginAtW):
    V=np.array([LocalXV,LocalYV,LocalZV])
    V=np.linalg.inv(V)
    LocalPuvn=np.dot(WorldPxyz-LocalOriginAtW,V)
    return LocalPuvn

def ToWorldCoordinate(LocalPuvn,LocalXV,LocalYV,LocalZV,LocalOriginAtW):
    V=np.array([LocalXV,LocalYV,LocalZV])
    WorldPxyz=LocalOriginAtW+np.dot(LocalPuvn,V)
    return WorldPxyz


'''
Ver1.0
  1. 
  
Ver2.0
  1. 加入Pynting Vector 显示

todo
 
  1. 加入曲面分解
  
'''
