#include "plane_mirror.h"

#include <vtkPlaneSource.h>
#include <vtkTransform.h>
#include <vtkTransformPolyDataFilter.h>
#include <vtkTriangleFilter.h>

PlaneMirror::PlaneMirror(const GraphTrans & _graphTrans)
{
	type = PLANEMIRROR;
	graphTrans = _graphTrans;
	data.resize(2);
	data[0] = 1;
	data[1] = 1;
	updateData();
}

PlaneMirror::PlaneMirror(const GraphTrans & _graphTrans, 
	const std::vector<double> parameter, double scale)
{
	type = PLANEMIRROR;
	graphTrans = _graphTrans;
	setData(parameter, scale);
	updateData();
}

void PlaneMirror::setParameter(GraphTrans & _graphTrans, std::vector<double> parameter) {
	type = PLANEMIRROR;
	graphTrans = _graphTrans;
	setData(parameter);
	updateData();
}

PlaneMirror::~PlaneMirror()
{
}

void PlaneMirror::calPolyData(vtkSmartPointer<vtkPolyData>& ptr, double ds)
{
	float w = data[0] / 2.0;
	float d = data[1] / 2.0;
	vtkSmartPointer<vtkPlaneSource> plane = vtkSmartPointer<vtkPlaneSource>::New();
	//plane->SetCenter(0, 0, 0);
	plane->SetOrigin(-w, -d, 0);
	plane->SetPoint1(w, -d, 0);
	plane->SetPoint2(-w, d, 0);
	plane->Update();

	ptr = plane->GetOutput();
	vtkSmartPointer<vtkTransform> transform = vtkSmartPointer<vtkTransform>::New();

	// 用户自定义平移旋转 (先移动后旋转)
	transform->Translate(graphTrans.getTrans_x(), 
		graphTrans.getTrans_y(), graphTrans.getTrans_z());
	for (int i = 0; i < 3; i++) {
		if (graphTrans.getRotate_x_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_x_theta(), 1, 0, 0);
		}
		else if (graphTrans.getRotate_y_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_y_theta(), 0, 1, 0);
		}
		else if (graphTrans.getRotate_z_num() == i) {
			transform->RotateWXYZ(graphTrans.getRotate_z_theta(), 0, 0, 1);
		}
	}

	vtkSmartPointer<vtkTransformPolyDataFilter> TransFilter =
		vtkSmartPointer<vtkTransformPolyDataFilter>::New();
	TransFilter->SetInputData(ptr);
	TransFilter->SetTransform(transform); //use vtkTransform (or maybe vtkLinearTransform)
	TransFilter->Update();
	ptr = TransFilter->GetOutput();
	vtkSmartPointer<vtkTriangleFilter> triangleFilter =
		vtkSmartPointer<vtkTriangleFilter>::New();
	triangleFilter->SetInputData(ptr);
	//triangleFilter->SetTransform(transform); //use vtkTransform (or maybe vtkLinearTransform)
	triangleFilter->Update();
	ptr = triangleFilter->GetOutput();
	polyData = triangleFilter->GetOutput();
}

void PlaneMirror::updateData()
{
	calcActorAxes();
	calActor();
}


QTreeWidgetItem * PlaneMirror::getTree()
{
	QTreeWidgetItem * tree = new QTreeWidgetItem;
	tree->setText(0, "PlaneMirror");
	tree->setData(0, Qt::UserRole, QVariant(1));
	tree->setData(1, Qt::UserRole, QVariant(PLANEMIRROR));

	QTreeWidgetItem * tree1 = new QTreeWidgetItem;
	tree1->setText(0, "PlaneMirror");
	tree1->setText(0, QString("Width: ") + QString::number(data[0]));

	QTreeWidgetItem * tree2 = new QTreeWidgetItem;
	tree2->setText(0, "PlaneMirror");
	tree2->setText(0, QString("Depth: ") + QString::number(data[1]));

	tree->addChild(tree1);
	tree->addChild(tree2);
	tree->addChild(getTransformTree());
	return tree;
}

void PlaneMirror::setWidth(double w)
{
	data[0] = w;
	updateData();
}

void PlaneMirror::setDepth(double d)
{
	data[1] = d;
	updateData();
}