#pragma once
#include "rotate_line_base.h"

class RotateLineKarman : public RotateLineBase {
public:
	RotateLineKarman();
	~RotateLineKarman();
	virtual void SetLineParamAndUpdate(const std::vector<double>& param) override;

private:

};