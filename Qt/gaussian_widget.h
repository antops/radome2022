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

class GaussianWidget : public GraphTransWidget
{
	Q_OBJECT

public:
	GaussianWidget(QWidget *parent = 0);
	virtual ~GaussianWidget();

	//void setWidgetData(Field const * _Source);
	//bool getWidgetData(Field * _Source,
	//	Parameterization* parameterization) const;

	// ������ʱ��ʾԴ��λ��
	bool getField(Field*&);

	// ������ʱ��ʾԴ��λ��
	void setMirror(Mirror*);

	private slots:
	void on_widthChange(QString var);
	void on_depthChange(QString var);

private:
	//void changeText(int index, const string& t);

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
	QLineEdit * RecLineEdit;

	//page3
	QGroupBox * imgGroupBox;

	QGroupBox * sourceGroupBox;
	QLabel * z0label;
	QLabel * w0label;
	QLabel * frelabel;
	QLabel * dslabel;

	QLineEdit * z0LineEdit;
	QLineEdit * w0LineEdit;
	QLineEdit * freLineEdit;
	QLineEdit * DsLineEdit;
	QLineEdit * dsLineEdit;
	QLabel* polarization_type_lable_;
	QComboBox* polarization_type_combobox_;

	QLabel* source_diff_lable_;
	QComboBox* source_diff_combobox_;

	PlaneMirror * planeMirror;

};

