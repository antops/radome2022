#pragma once
// ��ת�����ֵ���ת�ߵ�base class
#include <vector>
#include <string>
#include "../../common/include/Vector3.h"

using namespace Common;

class RotateLineBase {
public:
	RotateLineBase() {};
	virtual ~RotateLineBase() {};
	enum LineType
	{
		straight_line_type = 0, // ֱ��
		conic_line_type = 1, // ��������
		circular_straight_line_type = 2, // Բͷֱ��
		karman_line_type = 3, // �뿨������
		karman_line_radius_type = 4, // �뿨�ŵ�������
		import_point_type,   // �������ɢ��

	};
	virtual void SetLineParamAndUpdate(const std::vector<double>& param) {};
	virtual void SetLineParamAndUpdate(const std::string param) {};

	const std::vector<Vector3>& GetLines() { return lines_; }
	const std::vector<double>& GetParam() { return param_; }
	LineType GetLineType() { return type_; }

	bool SaveToFile(const std::string& path);

protected:
	// ��ת�ߣ���һ����ɢ����ɣ�
	std::vector<Vector3> lines_;
	LineType type_;
	//a0x^2 + a1x + a2 [a3,a4] a5������
	std::vector<double> param_;
};

class RotateLineBaseFactory {
public:
	static RotateLineBase* GenRotateLine(RotateLineBase::LineType);
};


