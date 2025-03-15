#pragma once

#include "../data_manager.h"
#include <vtkjsoncpp/json/json.h>

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

    ////0221:保存导入源的para
    void SetSourcePara(const std::vector<double>& para) {
       source_para_ = para;
    }
    void SetTransRotatePara(const std::vector<double>& para) {
       trans_rotate_para_ = para;
    }
	void SetN_width(int n_width) {
		n_width_ = n_width;	
	}
	void SetM_depth(int m_depth) {
		m_depth_ = m_depth;
	}
	
	////
	////0225:设置source_save_flag
	void SetSourceSaveFlag(int source_save_flag) {
		source_save_flag_ = source_save_flag;	
	}
	////
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
	GraphTrans graph_trans_;
	CalcConf conf_;
	////0221:保存导入源的para
	std::vector<double> source_para_;
	std::vector<double> trans_rotate_para_;
	int n_width_ = 0;
	int m_depth_ = 0;

	////
	////0225:设置source_save_flag
	int source_save_flag_ = 0;
	////
};
