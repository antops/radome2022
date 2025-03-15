#pragma once

#include <QSettings>


class GlobalConfig
{
public:
	static GlobalConfig* Instance() {
		static GlobalConfig config;
		return &config;
	}
	bool Init(const std::string& path);
	bool IsSkipQuickCalc() {
		return skip_quick_calc_;
	}
	bool IsSkipQuickCalcMetaGen() {
		return skip_quick_calc_meta_gen_;
	}
	const std::string& GetFarfieldPicName() {
		return farfield_pic_name_;
	}
	const bool IsQuickCalcByPython() {
		return is_quick_calc_by_python_;
	}
	const bool IsPlotByPython() {
		return is_plot_by_python_;
	}
	const int GraphicsThread() {
		return graphics_thread_;
	}

	const std::string& GetResultPath() {return  result_path_;}
	const std::string& GetNonRadomeReultPath() { return  non_radome_reult_path_; }

	bool IsSkipFDTDCalc() {
		return skip_FDTD_calc_;
	}
	const std::string& GetFDTDResultPath() { return  fdtd_result_path_; }
	const std::string& GetFDTDNonRadomeReultPath() { return  fdtd_non_radome_reult_path_; }
private:
	GlobalConfig() {}
	

private:
	bool skip_quick_calc_ = false;
	bool skip_quick_calc_meta_gen_ = false;
	std::string result_path_;
	std::string non_radome_reult_path_;
	std::string farfield_pic_name_ = "farField.jpg";


	bool skip_FDTD_calc_ = false;
	std::string fdtd_result_path_;
	std::string fdtd_non_radome_reult_path_;

	bool is_quick_calc_by_python_ = false;
	bool is_plot_by_python_ = false;
	int graphics_thread_ = 256;
};

