#pragma once

#include <QThread>
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include "../tools/gen_quick_calc_meta.h"
#include "../Qt/process_show_widget.h"
#include "../data_manager.h"

class QuickCalcThread : public QThread
{
	Q_OBJECT
public:
	QuickCalcThread(const DataManager& data_manager,
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

	void SetCalcConf(CalcConf conf) {
		conf_ = conf;
	}

	void SetPolarizationType(int polarization_type) {
		polarization_type_ = polarization_type;
	}

	void SetCustomPath(const std::string custom_path) {
		custom_path_ = custom_path;
	}


	const std::string GetResultPath() const {return result_path_;}
	const std::string GetResultNonRadomePath() const { return result_non_radome_path_; }
protected:
	void run() {

		// ��֤Դ
		auto source = data_manager_.GetSource();
		if (NULL == source) {
			status_ = -100;
			return;
		}


		// ��֤ģ��
		auto radome_data = data_manager_.GetRadomeData();
		if (radome_data == NULL) {
			status_ = -200;
			return;
		}

		// ��֤�����Ƿ����
		const auto& materials_index = radome_data->GetAllMaterials();
		for (auto index : materials_index) {
			if (data_manager_.GetMaterialData(index) == nullptr) {
				status_ = -201;
				return;
			}
		}

		process_show_widget_->QAppend(QString::fromLocal8Bit("��ʼ������..."));
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));
		GenQuickCalcMeta gen_meta(data_manager_);

		gen_meta.SetDs(data_manager_.GetSource()->getDs());
		gen_meta.SetGraphTrans(data_manager_.GetSource()->getGraphTrans());

		dir_path_ = QCoreApplication::applicationDirPath().toStdString() + "/output";
		QDir dir;
		dir.mkpath(dir_path_.c_str());


		std::string dir_stl_path = dir_path_ + "/stl/";
		dir.mkpath(dir_stl_path.c_str());

		QDateTime data_time = QDateTime::currentDateTime();
		QString current_time = data_time.toString("yyyy_MM_dd_hh_mm_ss");
		if (custom_path_.empty()) {
			custom_path_ = dir_path_ + "/result";
		}
		result_path_ = custom_path_ + "/" + current_time.toStdString() + "/";
		dir.mkpath(result_path_.c_str());
		gen_meta.SetResultPath(result_path_);

		if (is_calc_non_radome_) {
			result_non_radome_path_ = custom_path_ + "/nr_"+current_time.toStdString() + "/";
			dir.mkpath(result_non_radome_path_.c_str());
			gen_meta.SetResultNonRadomePath(result_non_radome_path_);
		}

		process_show_widget_->QAppend(QString::fromLocal8Bit("path:") 
			+ QString::fromLocal8Bit(dir_path_.c_str()));

		process_show_widget_->QAppend(QString::fromLocal8Bit("result path:")
			+ QString::fromLocal8Bit(custom_path_.c_str()));

		// ��������
		process_show_widget_->QAppend(QString::fromLocal8Bit("��ʼ����Դ����ȴ����..."));
		std::string source_path = dir_path_ + "/source.txt";
		source->SaveFieldBase(source_path);
		process_show_widget_->QAppend(QString::fromLocal8Bit("Դ�������..."));
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));

		// ����ģ��
		process_show_widget_->QAppend(QString::fromLocal8Bit("��ʼ����ģ�ͣ���ȴ����..."));
		std::vector<std::string> stl_path_vec;
		if (!radome_data->SaveToStl(dir_stl_path + "/", &stl_path_vec)) {
			status_ = -300;
			return;
		}
		process_show_widget_->QAppend(QString::fromLocal8Bit("ģ���������..."));
		process_show_widget_->QAppend(QString::fromLocal8Bit(" "));

		
		gen_meta.SetSourcePath(source_path);
		for (auto& x : stl_path_vec) {
			gen_meta.AddSTLPath(x);
		}
		gen_meta.SetOutputPath(dir_path_);
		gen_meta.SetFre(fre_);
		gen_meta.SetPolarizationType(polarization_type_);
		gen_meta.SetCalcConf(conf_);

		if (!gen_meta.WriteMetaMsg(dir_path_ + "/meta.json")) {
			status_ = -400;
			return;
		}
		
		status_ = 0;
		process_show_widget_->QAppend(QString::fromLocal8Bit("��ʼ���������..."));
	}
	ProcessShowWidget* process_show_widget_;
	const DataManager& data_manager_;

	int status_ = -1;
	std::string dir_path_;
	std::string result_path_;
	std::string result_non_radome_path_;
	std::string custom_path_;
	bool is_calc_non_radome_ = false;
	double fre_ = 0.0;
	int polarization_type_ = 1; // ��ֱ������1, ��1ˮƽ����
	CalcConf conf_;
};
