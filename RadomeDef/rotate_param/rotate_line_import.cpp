#include "rotate_line_import.h"

RotateLineImport::RotateLineImport()
{
	type_ = RotateLineBase::straight_line_type;
}

RotateLineImport::~RotateLineImport()
{
}

void RotateLineImport::SetLineParamAndUpdate(const std::vector<double>& param)
{
	for (int i = 0; i < param.size(); i += 2) {
		Vector3 v3;
		v3.z = param[i];
		v3.x = param[i + 1];
		v3.y = 0.0;
		lines_.push_back(v3);
	}
}
