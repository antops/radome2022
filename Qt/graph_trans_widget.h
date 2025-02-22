#pragma once

#include <QtWidgets/QDialog>
#include <QTabWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <Qlabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCloseEvent>
#include <QComboBox>
#include "../VTK/basic_parameters.h"
#include <vtkjsoncpp/json/json.h>

class GraphTransWidget : public QDialog
{
	Q_OBJECT

public:
	GraphTransWidget(QWidget *parent = 0);
	~GraphTransWidget();

	void addDefGroupBox(QGroupBox * _defGroupBox, QString filename);
	void addBaseGroupBox(QGroupBox * _baseGroupBox);
	void addTransGroupBox(QGroupBox * _baseGroupBox);
	void addRotateGroupBox(QGroupBox* _baseGroupBox);
	void addBtn(QGridLayout * _layoutbt, int wayButton = 0);

	void setMirror(BasicParameters*);

protected:
	void closeEvent(QCloseEvent *event);
	void SaveParam(Json::Value* param_js);
	bool LoadParam(const Json::Value& param_js);

	bool GetGraphTransOk() const {
		return is_param_ok_;
	}

private slots :
	void buttonClose();
	void buttonOk();
	void on_GraphChange(QString);
	void on_ComboboxChange(int);


signals:
	/*************************
	* 0: 表示close 不画任何图形
	* 1: 表示Create
	* 2: 表示更新
	**************************/
	void sendData(int);

private:
	BasicParameters* mirror;

	//page1
	QLabel * Ulabel;
	QLabel * Vlabel;
	QLabel * Nlabel;

	QLineEdit * ULineEidt;
	QLineEdit * VLineEidt;
	QLineEdit * NLineEidt;

	QLabel * translabel;
	QLineEdit * transLineEidt;

	QLabel* xthetalabel;
	QLineEdit* xthetaLineEidt;
	QLabel* ythetalabel;
	QLineEdit* ythetaLineEidt;
	QLabel* zthetalabel;
	QLineEdit* zthetaLineEidt;
	QGroupBox* RotateGroupxyzBox;

	QComboBox* x_theta_combobox_;
	QComboBox* y_theta_combobox_;
	QComboBox* z_theta_combobox_;

	QPushButton * createbtn;
	QPushButton * closebtn;

	bool is_param_ok_ = true;

};
