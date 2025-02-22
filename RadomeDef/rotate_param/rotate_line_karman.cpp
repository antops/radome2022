#include "rotate_line_karman.h"

RotateLineKarman::RotateLineKarman()
{
	type_ = RotateLineBase::karman_line_type;
}

RotateLineKarman::~RotateLineKarman()
{
}

void RotateLineKarman::SetLineParamAndUpdate(const std::vector<double>& param)
{
	
	param_ = param;
	int sampling = (int)(param_[5]);
	double step = (param_[4] - param_[1]) / sampling;
	lines_.clear();

	double threshold = param_[2] / sqrt(acos(-1));
	for (int i = 0; i < sampling; i++) {
		Vector3 v3;
		v3.z = param_[1] + step * i;
		double theta = acos(1 - (2 * step * i / param_[3]));
		v3.x = threshold * sqrt(theta - 0.5 * sin(2 * theta));
		v3.y = 0.0;
		lines_.push_back(v3);
	}

}

