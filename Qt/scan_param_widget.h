#pragma once

#include <map>
#include <QtWidgets/QDialog>
#include <QTabWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <Qlabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCloseEvent> 
#include "qcustomplot.h"

#include "../def.h"
#include "../VTK/farfield.h"
#include "process_show_widget.h"
#include "../data_manager.h"

#include "../def.h"

class ScanParamWidget : public QDialog
{
	Q_OBJECT

public:
	ScanParamWidget(DataManager* data_manager, int scan_widget_index, QWidget *parent = 0);
	~ScanParamWidget();

signals:

private slots :
	void OnOKClicked();

private:
	bool GenSource();
	bool QuickCalc();
	void GenSourceMeta();
	bool ReadScanParam();
	bool Islegal();
	bool InitEnv();
	void GenCalcMeta();
	bool GenTbl();
	bool ReadResult();
	void UpdateTable();
	bool ReadTheta();
	bool ReadPhi();
	CalcConf GetCalcConf() {
		CalcConf conf;
		conf.min_theta = min_theta_;
		conf.min_phi = min_phi_;
		conf.max_theta = max_theta_;
		conf.max_phi = max_phi_;
		conf.num_theta = num_theta_;
		conf.num_phi = num_phi_;
		return conf;
	}
private:
	QLabel* theta_label_;
	QLabel* phi_label_;
	QLabel* fre_label_ ;
	QLabel* radius_label_;
	QLabel* s11_label_;
	QLabel* dx_label_;
	QLabel* dy_label_;
	QLabel* mesh_N_label_;
	QLabel* ds_label_;

	QLineEdit* theta_edit_;
	QLineEdit* phi_edit_;
	QLineEdit* fre_edit_;
	QLineEdit* radius_edit_;
	QLineEdit* s11_edit_;
	QLineEdit* dx_edit_;
	QLineEdit* dy_edit_;
	QLineEdit* mesh_N_edit_;
	QLineEdit* ds_edit_;
	QGroupBox* basic_qbox_;
	QLabel* polarization_type_lable_;
	QComboBox* polarization_type_combobox_;

	QLabel* scan_param_label_;
	QComboBox* scan_param_combobox_;
	QLabel* min_label_;
	QLabel* max_label_;
	QLabel* step_label_;
	QLineEdit* min_edit_;
	QLineEdit* max_edit_;
	QLineEdit* step_edit_;
	QGroupBox* scan_qbox_;

	QPushButton* ok_qbt_;
	DataManager* data_manager_;
	ProcessShowWidget* process_show_widget_ = nullptr;

	QTableWidget* result_table_widget_;
	QGroupBox* result_qbox_;

	QCustomPlot* result_pic_plot_;
	QGroupBox* result_pic_qbox_;

	QVector<double> result_pic_x_;
	QVector<double> result_pic_y_;

	double theta_ = 0.0;
	double phi_ = 0.0;
	double fre_ = 17;
	double radius_ = 0.075;
	double s11_ = 30;
	double min_ = 0.0;
	double max_ = 90.0;
	double step_ = 30.0;
	double dx_ = 0.5;
	double dy_ = 0.5;
	double ds_ = 0.5;
	int mesh_N_ = 161;
	int polarization_type_ = 0;
	int scan_type_ = 0;
	int scan_count_;
	std::string result_path_;
	std::string dir_path_;

	std::string cur_result_path_;
	std::string cur_result_non_radome_path_;
	std::string cur_result_with_radome_path_;
	int cur_index_;
	int cur_value_;
	std::vector<std::string> stl_path_vec_;

	FarField* non_radome_far_field_;
	FarField* radome_far_field_;
	int scan_widget_index_;

	double min_y_ = 200;
	double max_y_ = 0;

	// farfield
	QLabel* theta_begin_lable_;
	QLineEdit* theta_begin_edit_;
	QLabel* theta_end_lable_;
	QLineEdit* theta_end_edit_;

	QLabel* phi_begin_lable_;
	QLineEdit* phi_begin_edit_;
	QLabel* phi_end_lable_;
	QLineEdit* phi_end_edit_;

	QLabel* theta_gap_lable_;
	QLineEdit* theta_gap_edit_;
	QLabel* phi_gap_lable_;
	QLineEdit* phi_gap_edit_;
	QGroupBox* farfield_box_;

	double min_theta_ = 0.0;
	double min_phi_ = 0.0;
	double max_theta_ = 180.0;
	double max_phi_ = 360.0;
	double gap_theta_ = 1.0;
	double gap_phi_ = 1.0;
	int num_theta_ = 181;
	int num_phi_ = 361;
};
