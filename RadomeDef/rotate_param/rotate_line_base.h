#pragma once
// 旋转天线罩的旋转线的base class
#include <vector>
#include <string>
#include "common/include/Vector3.h"

using namespace Common;

class RotateLineBase {
public:
	RotateLineBase() {};
	virtual ~RotateLineBase() {};
	enum LineType
	{
		straight_line_type = 0, // 直线
		conic_line_type = 1, // 二次曲线
		circular_straight_line_type = 2, // 圆头直线
		karman_line_type = 3, // 冯卡门曲线
		karman_line_radius_type = 4, // 冯卡门倒角曲线
		import_point_type,   // 导入的离散点

	};
	virtual void SetLineParamAndUpdate(const std::vector<double>& param) {};
	virtual void SetLineParamAndUpdate(const std::string param) {};

	const std::vector<Vector3>& GetLines() { return lines_; }
	const std::vector<double>& GetParam() { return param_; }
	LineType GetLineType() { return type_; }

	bool SaveToFile(const std::string& path);

protected:
	// 旋转线（由一堆离散点组成）
	std::vector<Vector3> lines_;
	LineType type_;
	//a0x^2 + a1x + a2 [a3,a4] a5采样率
	std::vector<double> param_;
};

class RotateLineBaseFactory {
public:
	static RotateLineBase* GenRotateLine(RotateLineBase::LineType);
};


