#include "rotate_line_straight.h"

RotateLineStaright::RotateLineStaright()
{
	type_ = RotateLineBase::straight_line_type;
}

RotateLineStaright::~RotateLineStaright()
{
}

void RotateLineStaright::SetLineParamAndUpdate(const std::vector<double>& param)
{
	RotateLineConic::SetLineParamAndUpdate(param);
}
