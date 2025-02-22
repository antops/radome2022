#pragma once
#include <vector>
#include "rotate_line_base.h"

class RotateLineConic : public RotateLineBase {
public:
	RotateLineConic();
	~RotateLineConic();

	virtual void SetLineParamAndUpdate(const std::vector<double>& param) override;

protected:

};