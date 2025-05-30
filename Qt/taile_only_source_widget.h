#pragma once

#include <QtWidgets/QDialog>
#include <QTabWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <Qlabel>
#include <QLineEdit>
#include <QPushButton>
#include <QComboBox>
#include "graph_trans_widget.h"

#include "../VTK/gaussain.h"
#include "../VTK/plane_mirror.h"

class TaileOnlySourceWidget : public GraphTransWidget
{
	Q_OBJECT

public:
	TaileOnlySourceWidget(QWidget *parent = 0);
	virtual ~TaileOnlySourceWidget();

	//void setWidgetData(Field const * _Source);
	//bool getWidgetData(Field * _Source,
	//	Parameterization* parameterization) const;

	// 用于临时显示源的位置
	bool getField(Field*&);

	// 用于临时显示源的位置
	void setMirror(Mirror*);

	private slots:
	void on_widthChange(QString var);
	void on_depthChange(QString var);
	void OnSourceDiffCombobox(int);

private:
	void InitSourceParam();
	bool ReadScanParam();
	void GenSourceMeta();
	bool GenSource();
	bool IsPythonWorkSuccess(const std::string& status_file);
	//void changeText(int index, const string& t);

	//page1
	QTabWidget * tabWidget;
	QGroupBox * defGroupBox;
	QGroupBox * baseGroupBox;
	QGroupBox * rotateGroupBox;
	QGroupBox * dimGroupBox;

	QLabel * widthlabel;
	QLabel * depthlabel;

	QLineEdit * widthLineEdit;
	QLineEdit * depthLineEdit;

	//page3
	QLabel* fre_label_;
	QLabel* radius_label_;
	QLabel* s11_label_;
	QLabel* dx_label_;
	QLabel* dy_label_;
	QLabel* ds_label_;
	QLabel* theta_label_;
	QLabel* phi_label_;

	QLineEdit* theta_edit_;
	QLineEdit* phi_edit_;
	QLineEdit* fre_edit_;
	QLineEdit* radius_edit_;
	QLineEdit* s11_edit_;
	QLineEdit* dx_edit_;
	QLineEdit* dy_edit_;
	QLineEdit* ds_edit_;
	QGroupBox* basic_qbox_;
	QLabel* polarization_type_lable_;
	QComboBox* polarization_type_combobox_;

	QLabel* source_diff_lable_;
	QComboBox* source_diff_combobox_;

	QLabel* zero_dep_lable_;
	QComboBox* zero_dep_combobox_;

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
	int source_diff_flag_ = 0;
	int zero_depth_d_ = 0;

	std::string dir_path_;

	PlaneMirror * planeMirror;

};

