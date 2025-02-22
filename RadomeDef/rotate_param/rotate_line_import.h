#pragma once
#include "rotate_line_base.h"

class RotateLineImport : public RotateLineBase {
public:
	RotateLineImport();
	~RotateLineImport();
	virtual void SetLineParamAndUpdate(const std::vector<double>& param) override;

private:

};