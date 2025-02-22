#pragma once

#include <QThread>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include "../tools/gen_quick_calc_meta.h"
#include "../Qt/process_show_widget.h"
#include "../data_manager.h"

class InitScanEnvThread : public QThread
{
	Q_OBJECT
public:
	InitScanEnvThread(const DataManager& data_manager,
		ProcessShowWidget* process_show_widget, 
		QObject * parent = 0) : data_manager_(data_manager), process_show_widget_(process_show_widget) {}

	int GetStatus() { return status_; }
	const std::string& GetDirPath() { return dir_path_; }
	const std::vector<std::string>& GetStlPathVec() {
		return stl_path_vec_;
	}

protected:
	void run() {
		// 验证模型
		auto radome_data = data_manager_.GetRadomeData();
		if (radome_data == NULL) {
			status_ = -200;
			return;
		}


		process_show_widget_->QAppend(QString::fromLocal8Bit("初始化环境..."));
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));
		dir_path_ = QCoreApplication::applicationDirPath().toStdString() + "/output";
		QDir dir;
		dir.mkpath(dir_path_.c_str());

		std::string dir_stl_path = dir_path_ + "/stl/";
		dir.mkpath(dir_stl_path.c_str());

		// 生成模型
		process_show_widget_->QAppend(QString::fromLocal8Bit("开始生成模型，请等待完成..."));
		;
		if (!radome_data->SaveToStl(dir_stl_path + "/", &stl_path_vec_)) {
			status_ = -300;
			return;
		}
		process_show_widget_->QAppend(QString::fromLocal8Bit("模型生成完成..."));
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));
		
		status_ = 0;
		process_show_widget_->QAppend(QString::fromLocal8Bit("初始化环境完成..."));
	}
	ProcessShowWidget* process_show_widget_;
	const DataManager& data_manager_;

	int status_ = -1;
	std::string dir_path_;
	std::vector<std::string> stl_path_vec_;

};
