#pragma once

#include <map>
#include <QtWidgets/QDialog>
#include <QTabWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <Qlabel>
#include <QComboBox>
#include <QPushButton>
#include <QCloseEvent> 
#include "qcustomplot.h"

#include "../RadomeDef/rotate_param/rotate_line_base.h"
#include "../def.h"
#include "../RadomeDef/material_data.h"

class RadomeParamDatailWidget : public QDialog
{
	Q_OBJECT

public:
	RadomeParamDatailWidget(QWidget *parent = 0);
	~RadomeParamDatailWidget();
	void SetParam(int index, int mat_id, const std::map<int, MaterialData*>&);
	int GetParam();
	const MaterialData& GetData() { return data_; }

signals:
	void UpdateRotateRadomeParam(int);

private slots :
	void OnOKClicked();

private:
	

private:
	QLabel * lable_;

	QComboBox* combo_box_;
	QGroupBox * param_box_;

	QPushButton* ok_bt_;
	int mat_id_;
	int index_ = -1;
	std::map<int, int> combox_2_mat_index_;
	MaterialData data_;
};
