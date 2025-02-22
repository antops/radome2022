#pragma once
#include <vector>
#include "rotate_line_base.h"

class RotateLineCircularStraight : public RotateLineBase {
public:
	RotateLineCircularStraight();
	~RotateLineCircularStraight();

	virtual void SetLineParamAndUpdate(const std::vector<double>& param) override;

protected:

};