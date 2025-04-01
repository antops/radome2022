#include "radome_data_base.h"
#include "vtkSTLWriter.h"

bool RadomeBaseData::GetMaxXYZ(Vector3 * min_p, Vector3 * max_p)
{
	if (polyData_vec_.empty()) return false;
 	// find the max box
	double x_max = VTK_DOUBLE_MIN;
	double y_max = VTK_DOUBLE_MIN;
	double z_max = VTK_DOUBLE_MIN;

	double x_min = VTK_DOUBLE_MAX;
	double y_min = VTK_DOUBLE_MAX;
	double z_min = VTK_DOUBLE_MAX;
	for (auto & data : polyData_vec_) {
		int EleNum = data->GetNumberOfCells();
		for (int i = 0; i < EleNum; i++)  //求与反射面的交点
		{
			vtkIdList * p;
			p = data->GetCell(i)->GetPointIds();
			double * point;
			point = data->GetPoint(p->GetId(0));
			if (x_min > point[0]) x_min = point[0];
			if (y_min > point[1]) y_min = point[1];
			if (z_min > point[2]) z_min = point[2];
			if (x_max < point[0]) x_max = point[0];
			if (y_max < point[1]) y_max = point[1];
			if (z_max < point[2]) z_max = point[2];
			point = data->GetPoint(p->GetId(1));
			if (x_min > point[0]) x_min = point[0];
			if (y_min > point[1]) y_min = point[1];
			if (z_min > point[2]) z_min = point[2];
			if (x_max < point[0]) x_max = point[0];
			if (y_max < point[1]) y_max = point[1];
			if (z_max < point[2]) z_max = point[2];
			point = data->GetPoint(p->GetId(1));
			if (x_min > point[0]) x_min = point[0];
			if (y_min > point[1]) y_min = point[1];
			if (z_min > point[2]) z_min = point[2];
			if (x_max < point[0]) x_max = point[0];
			if (y_max < point[1]) y_max = point[1];
			if (z_max < point[2]) z_max = point[2];
		}
	}
	min_p->x = x_min;
	min_p->y = y_min;
	min_p->z = z_min;
	max_p->x = x_max;
	max_p->y = y_max;
	max_p->z = z_max;
	return true;
}

bool RadomeBaseData::SaveToStl(const std::string & path, std::vector<std::string>* path_vec)
{
	if (is_need_update_) {
		UpdateSurface();
		is_need_update_ = false;
	}
	if (surface_poly_data_vec_.empty()) return false;
	for (int i = surface_poly_data_vec_.size() - 1; i >= 0; --i) {
		vtkSmartPointer<vtkSTLWriter> writer =
			vtkSmartPointer<vtkSTLWriter>::New();
		writer->SetInputData(surface_poly_data_vec_[i]);
		std::string filepath = path + "/radome" + std::to_string(i) + ".stl";
		writer->SetFileName(filepath.c_str());
		writer->Update();
		path_vec->emplace_back(filepath);
	}

	return true;
}

bool RadomeBaseData::SaveToStlVolume(const std::string& path, std::vector<std::string>* path_vec)
{
	if (polyData_vec_.empty()) return false;
	for (int i = polyData_vec_.size() - 1; i >= 0; --i) {
		vtkSmartPointer<vtkSTLWriter> writer =
			vtkSmartPointer<vtkSTLWriter>::New();
		writer->SetInputData(polyData_vec_[i]);
		std::string filepath = path + "/radome_v_" + std::to_string(i) + ".stl";
		writer->SetFileName(filepath.c_str());
		writer->Update();
		path_vec->emplace_back(filepath);
	}
	return true;
}

bool RadomeBaseData::SaveToPoints(const std::string& path, std::vector<std::string>* path_vec)
{
	return false;
}
