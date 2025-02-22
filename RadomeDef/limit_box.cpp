#include "limit_box.h"
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>

LimitBox::LimitBox()
{
	actor_property = vtkSmartPointer<vtkProperty>::New();
	actor_property->SetOpacity(0.2);
	actor_property->SetColor(187/255, 1, 1);
	actor = vtkSmartPointer<vtkActor>::New();
}

LimitBox::~LimitBox()
{
}

void LimitBox::SetParam(const Vector3& min_p, const Vector3& max_p) {
	min_p_ = min_p;
	max_p_ = max_p;
	vtkSmartPointer<vtkCubeSource> cube = vtkSmartPointer<vtkCubeSource>::New();
	cube->SetXLength(max_p_.x - min_p_.x);
	cube->SetZLength(max_p_.z - min_p_.z);
	cube->SetYLength(max_p_.y - min_p_.y);
	cube->SetCenter((max_p_.x + min_p_.x)/2, (max_p_.y + min_p_.y) / 2, (max_p_.z + min_p_.z) / 2);
	cube->Update();

	vtkSmartPointer<vtkPolyData> polyData = cube->GetOutput();
	vtkSmartPointer<vtkPolyDataMapper>mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
	mapper->SetInputData(polyData);

	actor->SetMapper(mapper);
	actor->SetProperty(actor_property);
}

vtkSmartPointer<vtkActor> LimitBox::GetActor() const
{
	return actor;
}

