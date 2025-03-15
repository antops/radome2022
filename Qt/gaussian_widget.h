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

	// 用于临时显示源的位置
	bool getField(Field*&);

	// 用于临时显示源的位置
	void setMirror(Mirror*);

	////0303
	int getM(){
		int M_depth = 1 / ds + 1;
		return M_depth;
	};
	int getN(){
		int N_width = 1 / ds + 1;
		return N_width;
	};
    ////
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

	PlaneMirror * planeMirror;
	////0303
	double ds;
	
	////
};

