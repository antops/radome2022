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

#include "../RadomeDef/rotate_param/rotate_line_base.h"
#include "../RadomeDef/material_data.h"
#include "../def.h"

class MaterialParamDatailWidget : public QDialog
{
	Q_OBJECT

public:
	MaterialParamDatailWidget(bool is_del = false, QWidget *parent = 0);
	~MaterialParamDatailWidget();
	void SetParam(const MaterialData& data, int index);
	double GetEps0() const { return eps_; }
	double GetMu0() const { return mu_; }
	double GetlossT() const { return lossT_; }
	const MaterialData& GetData() { return data_; }
signals:
	void UpdateMaterialRadomeParam(int);
	void DelMaterialRadomeParam(int);

private slots :
	void OnOKClicked();
	void OnDelClicked();
private:
	

private:
	QLabel * name_lable_;
	QLineEdit * name_edit_;
	QLabel * eps_lable_;
	QLineEdit * eps_edit_;
	QLabel * mu_lable_;
	QLineEdit * mu_edit_;
	QLabel * loss_lable_;
	QLineEdit * loss_edit_;
	QGroupBox * param_box_;

	QPushButton* ok_bt_;
	QPushButton* del_bt_;
	int index_ = -1;
	double eps_ = 0.0;
	double mu_ = 0.0;
	double lossT_ = 0.0;
	MaterialData data_;
};
