#pragma once

#include <QThread>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include "../tools/gen_FDTD_meta.h"
#include "../Qt/process_show_widget.h"
#include "../data_manager.h"

class FDTDCalcThread : public QThread
{
	Q_OBJECT
public:
	FDTDCalcThread(const DataManager& data_manager,
		ProcessShowWidget* process_show_widget, 
		QObject * parent = 0) : data_manager_(data_manager), process_show_widget_(process_show_widget) {}

	int GetStatus() { return status_; }
	const std::string& GetDirPath() { return dir_path_; }

	void SetCalcNonRadome(bool flag) {
		is_calc_non_radome_ = flag;
	}

	void SetFre(double fre) {
		fre_ = fre;
	}

	void SetMeshType(int mesh_type) {
		mesh_type_ = mesh_type;
	}

	const std::string GetResultPath() const {return result_path_;}
protected:
	void run() {

		// 验证源
		auto source = data_manager_.GetSource();
		if (NULL == source) {
			status_ = -100;
			return;
		}


		// 验证模型
		auto radome_data = data_manager_.GetRadomeData();
		if (radome_data == NULL) {
			status_ = -200;
			return;
		}

		// 验证材料是否添加
		const auto& materials_index = radome_data->GetAllMaterials();
		for (auto index : materials_index) {
			if (data_manager_.GetMaterialData(index) == nullptr) {
				status_ = -201;
				return;
			}
		}

		process_show_widget_->QAppend(QString::fromLocal8Bit("初始化环境..."));
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));
		GenFDTDCalcMeta gen_meta(data_manager_);


		dir_path_ = QCoreApplication::applicationDirPath().toStdString() + "/output/fdtd/";
		QDir dir;
		dir.mkpath(dir_path_.c_str());


		std::string dir_stl_path = dir_path_ + "/points/";
		dir.mkpath(dir_stl_path.c_str());

		QDateTime data_time = QDateTime::currentDateTime();
		QString current_time = data_time.toString("yyyy_MM_dd_hh_mm_ss");
		result_path_ = dir_path_ + "/fdtd_result/" + current_time.toStdString() + "/";
		dir.mkpath(result_path_.c_str());
		gen_meta.SetResultPath(result_path_);


		process_show_widget_->QAppend(QString::fromLocal8Bit("path:") 
			+ QString::fromLocal8Bit(dir_path_.c_str()));


		// 生成输入
		process_show_widget_->QAppend(QString::fromLocal8Bit("开始生成源，请等待完成..."));
		std::string source_path = dir_path_ + "/source.txt";
		source->SaveFieldBase(source_path);
		process_show_widget_->QAppend(QString::fromLocal8Bit("源生成完成..."));
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));

		// 生成模型
		process_show_widget_->QAppend(QString::fromLocal8Bit("开始生成模型，请等待完成..."));
		std::vector<std::string> point_path_vec;
		if (!radome_data->SaveToPoints(dir_stl_path + "/", &point_path_vec)) {
			status_ = -300;
			return;
		}
		process_show_widget_->QAppend(QString::fromLocal8Bit("模型生成完成..."));
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));

		
		gen_meta.SetSourcePath(source_path);
		gen_meta.SetPointPath(point_path_vec);
		gen_meta.SetOutputPath(dir_path_);
		gen_meta.SetFre(fre_);
		gen_meta.SetMeshType(mesh_type_);
		// gen_meta.SetPolarizationType(polarization_type_);

		gen_meta.WriteMetaMsg(dir_path_ + "/input.txt");
		
		status_ = 0;
		process_show_widget_->QAppend(QString::fromLocal8Bit("初始化环境完成..."));
	}
	ProcessShowWidget* process_show_widget_;
	const DataManager& data_manager_;

	int status_ = -1;
	std::string dir_path_;
	std::string result_path_;
	bool is_calc_non_radome_ = false;
	double fre_ = 0.0;
	int mesh_type_ = 0;

};
