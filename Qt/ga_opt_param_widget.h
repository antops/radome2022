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

#include "../RadomeDef/ga_params.h"
#include "../def.h"

class GaOptParamWidget : public QDialog
{
	Q_OBJECT

public:
	GaOptParamWidget(int radome_num = 2, QWidget *parent = 0);
	~GaOptParamWidget();
	const GaParams& GetGaParams() {
		return param_;
	}

private slots :
	void OnOKClicked();
	void OnDelClicked();
private:
	struct RadomeGaParamsQt {
		QLabel* d_lable_;
		QLineEdit* d_min_edit_;
		QLineEdit* d_max_edit_;
		QLabel* e_lable_;
		QLineEdit* e_min_edit_;
		QLineEdit* e_max_edit_;
		QGroupBox* param_box_;
		QLabel* index_lable_;
		bool ParseRadomeGaParams(double* d_max,
			double* d_min,
			double* e_min,
			double* e_max);
	};
	
	void InitRadomeGaParams(RadomeGaParamsQt* param, int index);
	bool ParseRadomeGaParams();
private:
	QLabel * size_pop_lable_;
	QLineEdit * size_pop_edit_;
	QLabel * prob_mut_lable_;
	QLineEdit * prob_mut_edit_;
	QLabel * max_iter_lable_;
	QLineEdit * max_iter_edit_;
	QTabWidget* tab_widget_;
	
	std::vector<RadomeGaParamsQt> params_vec_;

	QGroupBox* opt_param_box_;
	QGroupBox* model_param_box_;

	QPushButton* ok_bt_;
	QPushButton* cancel_bt_;

	GaParams param_;
};
