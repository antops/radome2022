#pragma once

#include "../data_manager.h"
#include <vtkjsoncpp/json/json.h>
#include "def.h"

class GenQuickCalcMeta
{
public:
	explicit GenQuickCalcMeta(const DataManager& data_manager) : data_manager_(data_manager){}
	~GenQuickCalcMeta() {}

	bool WriteMetaMsg(const std::string& path);
	void AddSTLPath(const std::string& path) {
		stl_path_.emplace_back(path);
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

	void SetDs(double ds) {
		ds_ = ds;
	}

	void SetPolarizationType(int polarization_type) {
		polarization_type_ = polarization_type;
	}

	void SetGraphTrans(const GraphTrans& graph_trans) {
		graph_trans_ = graph_trans;
	}

	void SetCalcConf(CalcConf conf) {
		conf_ = conf;
	}

	void SetSourceDiffFlag(int source_diff_flag) {
		source_diff_flag_ = source_diff_flag;
	}

	void SetZeroDepthD(int zero_depth_d) {
		zero_depth_d_ = zero_depth_d;
	}

private:
	const DataManager& data_manager_;
	std::vector<std::string> stl_path_;
	std::string source_path_;
	std::string output_path_;
	bool is_calc_non_radome_ = false;	
	std::string result_path_;
	std::string result_non_radome_path_;
	double fre_ = 0.0;
	double ds_ = 0.0;
	int polarization_type_ = 1; // 垂直极化是1, 非1水平极化
	int source_diff_flag_ = 0;
	int zero_depth_d_ = 0; // 差波束时的0深方向，0为X，1为Y
	GraphTrans graph_trans_;
	CalcConf conf_;
};
