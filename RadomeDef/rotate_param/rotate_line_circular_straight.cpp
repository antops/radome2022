#include "rotate_line_circular_straight.h"
#include "../../common/include/Constant_Var.h"

RotateLineCircularStraight::RotateLineCircularStraight()
{
	type_ = RotateLineBase::circular_straight_line_type;
}

RotateLineCircularStraight::~RotateLineCircularStraight()
{
}

void RotateLineCircularStraight::SetLineParamAndUpdate(const std::vector<double>& param)
{
	param_ = param;
	// x0 theta r h
	double x0 = param_[0];
	double theta = param_[1];
	double r = param_[2];
	double h = param_[3];
	double sin_theta = sin(theta / 180 * Pi);
	double tan_theta = tan(theta / 180 * Pi);
	double cos_theta = cos(theta / 180 * Pi);
	double cir_x_max = r - r * sin_theta;

	double tmp = r * cos_theta / tan_theta;
	double line_x_max = h / tan_theta - tmp;

	int sampling = (int)(param_[4]);
	int sampling_cir = sampling * cir_x_max / (line_x_max);
	double step_cir = (90 - theta) / sampling_cir;
	lines_.clear();

	// Ô²
	for (int i = 0; i < sampling_cir; i++) {
		Vector3 v3;
		double x = r - cos((step_cir * i) / 180 * Pi) * r;
		v3.z = x0 + x;
		v3.y = 0.0;
		v3.x = sqrt((r*r - (x - r)*(x - r)));
		lines_.push_back(v3);
	}

	// Ïß
	int sampling_line = sampling - sampling_cir;
	double step_sta = line_x_max / sampling;
	for (int i = 0; i < sampling; i++) {
		Vector3 v3;
		double x = step_sta * i;
		v3.z = x0 + x + cir_x_max;
		v3.x = (x + tmp)*tan_theta;
		v3.y = 0.0;
		lines_.push_back(v3);
	}
}
