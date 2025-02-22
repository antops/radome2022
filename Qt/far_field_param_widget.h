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

class FarFieldParamWidget : public QDialog
{
	Q_OBJECT

public:
	FarFieldParamWidget(bool is_del = false, QWidget *parent = 0);
	~FarFieldParamWidget();
	void SetFarFieldParam(const FarFieldParam& param) { param_ = param; }
	const FarFieldParam& GetFarFieldParam() { return param_; }

signals:
	void UpdateFarField();

private slots :
	void OnSwitchClickedUnit();
	void MaxChanged(int val);
	void MinChanged(int val);
	void OnButtonUpdate();
private:
	

private:
	QButtonGroup * switch_group_button_unit_;
	QRadioButton * linear_button_unit_;
	QRadioButton * dB_button_unit_;

	QLabel* max_label_;
	QLabel* max_show_label_;
	QLabel* min_label_ ;
	QLabel* min_show_label_;


	QScrollBar* max_scroll_bar_;
	QScrollBar* min_scroll_bar_;
	QPushButton* update_btn_;

	FarFieldParam param_;

};
