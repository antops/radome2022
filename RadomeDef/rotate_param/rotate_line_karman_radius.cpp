#include "rotate_line_karman_radius.h"

RotateLineKarmanRadius::RotateLineKarmanRadius()
{
	type_ = RotateLineBase::karman_line_radius_type;
}

RotateLineKarmanRadius::~RotateLineKarmanRadius()
{
}

void RotateLineKarmanRadius::SetLineParamAndUpdate(const std::vector<double>& param)
{
	
	param_ = param;
	int sampling = (int)(param_[5]);
	double step = (param_[4] - param_[1]) / sampling;
	lines_.clear();

	double threshold = param_[2] / sqrt(acos(-1));


	std::vector<double> r_data;
	for (int i = 0; i < sampling; i++) {
		Vector3 v3;
		v3.z = param_[1] + step * i;
		double theta = acos(1 - (2 * step * i / param_[3]));
		double theta_dao = -1 / sqrt(1- pow((1 - (2 * step * i / param_[3])), 2)) * (-2/ param_[3]);
		// 导数
		double grad = threshold * theta_dao * (1 - cos(2 * theta)) / 2 / sqrt(theta - 0.5 * sin(2 * theta));
		double scan_x = threshold * sqrt(theta - 0.5 * sin(2 * theta));
		double tip_z = grad * scan_x + step * i;
		double radius = sqrt(scan_x * scan_x + pow(tip_z - step * i, 2));
		v3.x = radius;
		v3.y = 0.0;
		r_data.emplace_back(radius);
	}
	// 圆心 tip_z
	double target_r = param_[6];
	int index = BinarySearch(r_data, target_r);
	if (index < 0) return;
	double z0 = 0.0;
	{
		double theta = acos(1 - (2 * step * index / param_[3]));
		double theta_dao = -1 / sqrt(1 - pow((1 - (2 * step * index / param_[3])), 2)) * (-2 / param_[3]);
		double grad = threshold * theta_dao * (1 - cos(2 * theta)) / 2 / sqrt(theta - 0.5 * sin(2 * theta));
		double scan_x = threshold * sqrt(theta - 0.5 * sin(2 * theta));
		z0 = grad * scan_x + step * index;
	}

	for (int i = 0; i < sampling; i++) {
		Vector3 v3;
		v3.z = param_[1] + step * i;
		v3.y = 0.0;
		if (i <= index) {
			v3.x = sqrt(target_r * target_r - pow((step * i - z0), 2));
			if (std::isnan(v3.x))continue;
		}
		
		else {
			double theta = acos(1 - (2 * step * i / param_[3]));
			v3.x = threshold * sqrt(theta - 0.5 * sin(2 * theta));
		}
		lines_.push_back(v3);
	}
}

int RotateLineKarmanRadius::BinarySearch(const std::vector<double>& r_data, double target) {
	int low = 0;
	int high = r_data.size() - 1;
	int closestIndex = -1; // 用于存储最接近值的索引  
	double closestDiff = INT_MAX; // 用于存储当前最小差值  

	while (low <= high) {
		int mid = low + (high - low) / 2;
		double diff = std::abs(r_data[mid] - target);

		// 如果找到目标值，直接返回  
		if (r_data[mid] == target) {
			return mid;
		}

		// 更新最接近值的索引和差值  
		if (diff < closestDiff) {
			closestIndex = mid;
			closestDiff = diff;
		}

		// 根据目标值与中间值的比较结果来缩小搜索范围  
		if (r_data[mid] < target) {
			low = mid + 1;
		}
		else {
			high = mid - 1;
		}
	}

	// 如果没有找到目标值，返回最接近值的索引  
	return closestIndex;
}

