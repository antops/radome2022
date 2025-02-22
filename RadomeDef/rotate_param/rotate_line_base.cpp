#include "rotate_line_base.h"
#include "rotate_line_conic.h"
#include "rotate_line_straight.h"
#include "rotate_line_circular_straight.h"
#include "rotate_line_import.h"
#include "rotate_line_karman.h"
#include "rotate_line_karman_radius.h"

#include <fstream>

RotateLineBase * RotateLineBaseFactory::GenRotateLine(RotateLineBase::LineType type)
{
	switch (type)
	{
	case RotateLineBase::conic_line_type:
		return new RotateLineConic;
	case RotateLineBase::straight_line_type:
		return new RotateLineStaright;
	case RotateLineBase::circular_straight_line_type:
		return new RotateLineCircularStraight;
	case RotateLineBase::karman_line_type:
		return new RotateLineKarman;
	case RotateLineBase::import_point_type:
		return new RotateLineImport;
	case RotateLineBase::karman_line_radius_type:
		return new RotateLineKarmanRadius;
	default:
		break;
	}
	return nullptr;
}

bool RotateLineBase::SaveToFile(const std::string& path)
{
	std::ofstream outfile(path);
	if (!outfile.is_open())
	{
		return false;
	}
	for (auto& point : lines_) {
		outfile << point.z << " " << point.x << std::endl;
	}
	outfile.close();
}
