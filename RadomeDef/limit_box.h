#pragma once

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkSmartPointer.h>
#include <vtkProperty.h>
#include "common/include/Vector3.h"
using namespace Common;

class LimitBox
{
public:
	LimitBox();
	~LimitBox();

	void SetParam(const Vector3& min_p, const Vector3& max_p);

	vtkSmartPointer<vtkActor> GetActor() const;

private:
	vtkSmartPointer<vtkActor> actor;

	bool isTransparent; //  «∑Òœ‘ æ
	vtkSmartPointer<vtkProperty> actor_property;

	Vector3 min_p_;
	Vector3 max_p_;
};
