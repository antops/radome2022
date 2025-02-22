#pragma once

#include "../data_manager.h"


class CalcTbl
{
public:
	explicit CalcTbl(FarField* far_field_a, FarField* far_field_b)
		: far_field_a_(far_field_a), far_field_b_(far_field_b) {}
	~CalcTbl() {}

	bool Run();

private:
	FarField* far_field_a_;
	FarField* far_field_b_;
};
