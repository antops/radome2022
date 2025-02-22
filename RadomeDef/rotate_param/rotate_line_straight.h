#pragma once
#include "rotate_line_conic.h"

class RotateLineStaright : public RotateLineConic {
public:
	RotateLineStaright();
	~RotateLineStaright();
	virtual void SetLineParamAndUpdate(const std::vector<double>& param) override;

private:

};