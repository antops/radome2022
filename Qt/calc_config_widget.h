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

private slots :
	void OnOKClicked();
	

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

	QPushButton* ok_bt_;
	double fre_ = 10.0;
	bool is_calc_nonrodome_ = true;
	int polarization_type_ = 1;
};
