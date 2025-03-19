#pragma once

#include "../data_manager.h"
#include <vtkjsoncpp/json/json.h>

class GenFDTDCalcMeta
{
public:
	explicit GenFDTDCalcMeta(const DataManager& data_manager) : data_manager_(data_manager){}
	~GenFDTDCalcMeta() {}

	bool WriteMetaMsg(const std::string& path);
	void SetPointPath(const std::vector<std::string>& point_path) {
		point_path_ = point_path;
	}

	void SetSourcePath(const std::string& path) {
		source_path_ = path;
	}

	void SetOutputPath(const std::string& path) {
		output_path_ = path;
	}

	void SetResultPath(const std::string& path) {
		result_path_ = path;
	}

	void SetResultNonRadomePath(const std::string& path) {
		result_non_radome_path_ = path;
		is_calc_non_radome_ = true;
	}

	void SetFre(double fre) {
		fre_ = fre;
	}

	void SetMeshType(int mesh_type) {
		mesh_type_ = mesh_type;
	}


	void SetPolarizationType(int polarization_type) {
		polarization_type_ = polarization_type;
	}

private:
	const DataManager& data_manager_;
	std::vector<std::string> point_path_;
	std::string source_path_;
	std::string output_path_;
	bool is_calc_non_radome_ = false;	
	std::string result_path_;
	std::string result_non_radome_path_;
	double fre_ = 0.0;
	int mesh_type_ = 0; // 0:1/20; 1:1/25; 2:1/30; 3:1/40
	int polarization_type_ = 1; // 垂直极化是0, 1水平极化
	int cpu_core_ = 22;
};
