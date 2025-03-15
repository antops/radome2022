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




class FDTDCalcConfigWidget : public QDialog
{
	Q_OBJECT

public:
	FDTDCalcConfigWidget(double fre_, QWidget *parent = 0);
	~FDTDCalcConfigWidget() {}

	bool GetIsCalcNonrodome() const {
		return is_calc_nonrodome_;
	}

	double GetFre() const {
		return fre_;
	}

	double GetMeshType() const {
		return mesh_type_;
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

	QLabel* mesh_type_lable_;
	QComboBox* mesh_type_combobox_;

	QPushButton* ok_bt_;
	double fre_ = 10.0;
	bool is_calc_nonrodome_ = true;
	int mesh_type_ = 0;
};
