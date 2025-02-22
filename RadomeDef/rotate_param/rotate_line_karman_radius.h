#pragma once
#include "rotate_line_base.h"

// ´øµ¹½ÇµÄ·ë¿¨ÃÅ
class RotateLineKarmanRadius : public RotateLineBase {
public:
	RotateLineKarmanRadius();
	~RotateLineKarmanRadius();
	virtual void SetLineParamAndUpdate(const std::vector<double>& param) override;
	int BinarySearch(const std::vector<double>& r_data, double target);

private:

};