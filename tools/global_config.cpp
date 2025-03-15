#include "global_config.h"
#include <QTextCodec>

bool GlobalConfig::Init(const std::string& path) {
	QSettings settings(path.c_str(), QSettings::IniFormat);

	if (settings.value("Debug/debug_enable").toInt() == 1) {
		skip_quick_calc_ = settings.value("Debug/skip_quick_calculate").toInt() == 1;

		skip_quick_calc_meta_gen_ = settings.value("Debug/skip_quick_calc_meta_gen").toInt() == 1;

		result_path_ = settings.value("Debug/result_path").toString().toStdString();
		non_radome_reult_path_ = settings.value("Debug/non_radome_reult_path").toString().toStdString();

		skip_FDTD_calc_ = settings.value("Debug/skip_fdtd_calculate").toInt() == 1;
		fdtd_result_path_ = settings.value("Debug/fdtd_result_path").toString().toStdString();
		fdtd_non_radome_reult_path_ = settings.value("Debug/fdtd_non_radome_reult_path").toString().toStdString();
	}

	farfield_pic_name_ = settings.value("Debug/farfield_pic_name", farfield_pic_name_.c_str()).toString().toStdString();
	is_quick_calc_by_python_ = settings.value("sys/quick_calc_by_python").toInt() == 1;

	is_plot_by_python_ = settings.value("sys/plot_by_python").toInt() == 1;

	graphics_thread_ = settings.value("sys/graphics_thread", graphics_thread_).toInt();


	
	return true;
}