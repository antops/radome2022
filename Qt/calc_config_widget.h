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

class CalcConfigWidget : public QDialog
{
	Q_OBJECT

public:
	CalcConfigWidget(double fre_, QWidget *parent = 0);
	~CalcConfigWidget() {}

	bool GetIsCalcNonrodome() const {
		return is_calc_nonrodome_;
	}

	double GetFre() const {
		return fre_;
	}

	double GetPolarizationType() const {
		return polarization_type_;
	}

	bool ReadTheta();
	bool ReadPhi();
	bool ReadPath();

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

	const std::string& GetCustomPath() const {
		return custom_path_;
	}

private slots :
	void OnOKClicked();
	void OnThetaChange();
	void OnPhiChange();
	void OnBrowseBtn();

private:
	QLabel * fre_lable_;
	QLineEdit * fre_edit_;
	QLabel * is_calc_nonrodome_lable_;
	QButtonGroup * switch_group_button_;
	QRadioButton * yes_unit_;
	QRadioButton * no_unit_;
	QGroupBox * param_box_;

	QLabel* polarization_type_lable_;
	QComboBox* polarization_type_combobox_;

	QLabel* theta_begin_lable_;
	QLineEdit* theta_begin_edit_;
	QLabel* theta_end_lable_;
	QLineEdit* theta_end_edit_;
	QGroupBox* begin_box_;

	QLabel* phi_begin_lable_;
	QLineEdit* phi_begin_edit_;
	QLabel* phi_end_lable_;
	QLineEdit* phi_end_edit_;
	QGroupBox* end_box_;

	QLabel* theta_gap_lable_;
	QLineEdit* theta_gap_edit_;
	QLabel* phi_gap_lable_;
	QLineEdit* phi_gap_edit_;
	QGroupBox* gap_box_;

	QLabel* theta_num_lable_;
	QLineEdit* theta_num_edit_;
	QLabel* phi_num_lable_;
	QLineEdit* phi_num_edit_;
	QGroupBox* num_box_;

	QLabel* path_lable_;
	QLineEdit* path_edit_;
	QPushButton* browse_btn_;


	QPushButton* ok_bt_;
	double fre_ = 10.0;
	bool is_calc_nonrodome_ = true;
	int polarization_type_ = 1;

	double min_theta_ = 0.0;
	double min_phi_ = 0.0;
	double max_theta_ = 180.0;
	double max_phi_ = 360.0;
	double gap_theta_ = 1.0;
	double gap_phi_ = 1.0;
	int num_theta_ = 181;
	int num_phi_ = 361;

	std::string custom_path_;
};
