#include "rotate_line_conic.h"

RotateLineConic::RotateLineConic()
{
	type_ = RotateLineBase::conic_line_type;
}

RotateLineConic::~RotateLineConic()
{
}

void RotateLineConic::SetLineParamAndUpdate(const std::vector<double>& param)
{
	param_ = param;
	int sampling = (int)(param_[5]);
	double step = (param_[4] - param_[3]) / sampling;
	lines_.clear();

	for (int i = 0; i < sampling; i++) {
		Vector3 v3;
		v3.z = param_[3] + step * i;
		v3.x = param_[0] * v3.z * v3.z + param_[1] * v3.z + param_[2];
		v3.y = 0.0;
		lines_.push_back(v3);
		if (i == 0) {
			Vector3 v3_tmp;
			v3_tmp.x = 0.0;
			v3_tmp.y = 0.0;
			v3_tmp.z = v3.z;
			//lines_.push_back(v3_tmp);
		}
		else if (i == sampling - 1) {
			Vector3 v3_tmp;
			v3_tmp.x = 0.0;
			v3_tmp.y = 0.0;
			v3_tmp.z = v3.z;
			//lines_.push_back(v3_tmp);
		}
	}

}
