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
#include "aperture_field_widget.h"

#include "../VTK/plane_mirror.h"
#include "../VTK/field.h"



class ApertureFieldWidget : public GraphTransWidget
{
	Q_OBJECT

public:
	ApertureFieldWidget(QWidget *parent = 0);
	~ApertureFieldWidget();

	bool getField(Field *& ptr);

	// 用于临时显示源的位置
	void setMirror(Mirror*);

	////0221：保存源参数
	void getParameter(std::vector<double>& parameter);
	int getM();
	int getN();
	// double getFre();
	////
	private slots:
	void EfileEeven();

	void on_widthChange(QString var);
	void on_depthChange(QString var);

	void SaveParam(Json::Value* param_js);
	void LoadParam(const Json::Value& param_js);
	
private:

	//page1
	QTabWidget * tabWidget;
	QGroupBox * defGroupBox;
	QGroupBox * baseGroupBox;
	QGroupBox* rotateGroupBox;
	QGroupBox * dimGroupBox;

	QLabel * widthlabel;
	QLabel * depthlabel;

	QLineEdit * widthLineEdit;
	QLineEdit * depthLineEdit;

	QLabel * label;
	QLineEdit * RecLineEidt;

	//page3
	QGroupBox * imgGroupBox;

	QLabel * scalelabel;
	QLabel * phaselabel;
	QLabel * datadeflabel;
	QLabel * fre_label_;

	QLineEdit * scaleLineEidt;
	QLineEdit * phaseLineEidt;
	QLineEdit * fre_line_edit_;
	QComboBox * datadefComboBox;

	QGroupBox * sourceGroupBox;
	QLabel * Efieldlabel;
	QLabel * StartNumlabel;
	QLabel * StartNumColumnlabel;

	QPushButton * Efieldbtn;

	QLineEdit * EfieldLineEidt;
	QLineEdit * StartNumLineEidt;
	QLineEdit * StartNumColumnEidt;

	QGroupBox * destinationGroupBox;
	QLabel * UNumlabel;
	QLabel * VNumlabel;

	QLineEdit * UNumLineEidt;
	QLineEdit * VNumLineEidt;

	PlaneMirror * planeMirror;
	////0221：保存源参数
	//vector<double> para(14);
	std::vector<double> para = std::vector<double>(14);
	int N, M;
	double fre;
	////
};
