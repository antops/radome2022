#include "mirror.h"
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>

#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>

#include <vtkSTLWriter.h>
#include "../Calculation/RayTracing.h"
#include <vtkFloatArray.h>
#include <vtkPoints.h>
#include <vtkPointData.h>
#include <vtkDelaunay2D.h>
#include <vtkTransform.h>
#include <vtkLine.h>
Mirror::Mirror()
{
	property = vtkSmartPointer<vtkProperty>::New();
	property->SetOpacity(1);
	property->SetColor(180.0 / 255.0, 180.0 / 255.0, 180.0 / 255.0);
	//property->SetColor(1, 1, 0);
	actor = vtkSmartPointer<vtkActor>::New();
	actorAxes = vtkSmartPointer<vtkAxesActor>::New();
	isTransparent = false;
	isShow = true;
	transEffi = 1.0;
	ismeshed = false;
}

Mirror::~Mirror()
{
}

MirrorsType Mirror::getMirrorsType() const
{
	return type;
}

vtkSmartPointer<vtkActor> Mirror::getActor() const
{
	return actor;
}

void Mirror::calActor()
{
	vtkSmartPointer<vtkPolyData> polyDataPtr;
	calPolyData(polyDataPtr);
	vtkSmartPointer<vtkPolyDataMapper>mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyDataPtr);
	mapper->ScalarVisibilityOff();

	actor->SetMapper(mapper);
	actor->SetProperty(property);
}

vtkSmartPointer<vtkPolyData> Mirror::getPolyData() const
{
	return polyData;
}

void Mirror::setSelected(bool flag = true)
{
	//actor->GetProperty()->SetOpacity(0.5);
	//actor->GetProperty()->SetColor(0, 1, 0);
	if (flag)
	{
		property->SetOpacity(0.2);
		property->SetColor(0, 0, 1);
	}
	else
	{
		property->SetOpacity(1);
		property->SetColor(180.0 / 255.0, 180.0 / 255.0, 180.0 / 255.0);
	}

}



void Mirror::switchIsTransparent()
{
	isTransparent = !isTransparent;
	if (!isShow)
		return;
	if (isTransparent) // 透明 
	{
		property->SetOpacity(0.3);
	}
	else
	{
		property->SetOpacity(1);
	}
}

void Mirror::setIsTransparent(bool tmp)
{
	isTransparent = tmp;
	if (!isShow)
		return;
	if (isTransparent) // 透明 
	{
		property->SetOpacity(0.3);
	}
	else
	{
		property->SetOpacity(1);
	}
}

void Mirror::switchIsShow()
{
	isShow = !isShow;
	if (isShow) // 显示 
	{
		if (isTransparent) // 透明 
		{
			property->SetOpacity(0.3);
		}
		else
		{
			property->SetOpacity(1);
		}
	}
	else
	{
		property->SetOpacity(0);
	}
}

void Mirror::setIsShow(bool tmp)
{
	isShow = tmp;
	if (isShow) // 显示 
	{
		if (isTransparent) // 透明 
		{
			property->SetOpacity(0.3);
		}
		else
		{
			property->SetOpacity(1);
		}
	}
	else
	{
		property->SetOpacity(0);
	}
}

vtkSmartPointer<vtkAxesActor> Mirror::getActorAxes() const
{
	return actorAxes;
}

void Mirror::calcActorAxes()
{
	actorAxes->GetXAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(1, 0, 0);//修改X字体颜色为红色  
	actorAxes->GetYAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 2, 0);//修改Y字体颜色为绿色  
	actorAxes->GetZAxisCaptionActor2D()->GetCaptionTextProperty()->SetColor(0, 0, 3);//修改Z字体颜色为蓝色  
	actorAxes->SetXAxisLabelText("U");
	actorAxes->SetYAxisLabelText("V");
	actorAxes->SetZAxisLabelText("N");
	actorAxes->SetConeRadius(0.1);
	actorAxes->SetConeResolution(20);
	actorAxes->SetTotalLength(0.5, 0.5, 0.5);

	vtkSmartPointer<vtkTransform> transform = 
		vtkSmartPointer<vtkTransform>::New();

	// 用户自定义平移旋转 (先移动后旋转)
	transform->Translate(graphTrans.getTrans_x(),
		graphTrans.getTrans_y(), graphTrans.getTrans_z());
	transform->RotateWXYZ(graphTrans.getRotate_theta(), 
		graphTrans.getRotate_x(), graphTrans.getRotate_y(),
		graphTrans.getRotate_z());

	actorAxes->SetUserTransform(transform);
}

void Mirror::saveSTL()
{
	// 改
	vtkSmartPointer<vtkSTLWriter> writer =
		vtkSmartPointer<vtkSTLWriter>::New();
	//calPolyData(polyData, 0.01);
	writer->SetInputData(polyData);
	writer->SetFileName("test.stl");
	writer->Update();
}


void Mirror::saveSTLfilename(std::string _filename) {
	// 改
	vtkSmartPointer<vtkSTLWriter> writer =
		vtkSmartPointer<vtkSTLWriter>::New();
	vtkSmartPointer<vtkPolyData> polyDataPtr;

	if (ismeshed) {
		writer->SetInputData(polyData);
	}
	else {
		calPolyData(polyDataPtr);
	    writer->SetInputData(polyDataPtr);
	}
	//calPolyData(polyData, 0.01);

	writer->SetFileName(_filename.c_str());
	writer->Update();
}


void Mirror::genMesh(double ds)
{
	calPolyData(polyData, ds);
	ismeshed = true;
}

void Mirror::setTransEffi(double tmp)
{
	if (tmp > 1.0 || tmp < 0.0001)
	{
		transEffi = 1.0;
		return;
	}
	transEffi = tmp;
}

