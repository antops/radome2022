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
	void setMirror_load(BasicParameters*, std::vector<double> parameter);
    ////0305
	void GetTransRotate(std::vector<double>& parameter){
       	parameter.resize(9);
		parameter[0] = tran_x;
		parameter[1] = tran_y;
		parameter[2] = tran_z;
		parameter[3] = rotate_x_theta;
		parameter[4] = rotate_y_theta;
		parameter[5] = rotate_z_theta;
		parameter[6] = x_index;
		parameter[7] = y_index;
		parameter[8] = z_index;
	};
	void SetTransRotate(std::vector<double> parameter){
		parameter.resize(9);
		tran_x = parameter[0];
		tran_y = parameter[1];
		tran_z = parameter[2];
		rotate_x_theta = parameter[3];
		rotate_y_theta = parameter[4];
		rotate_z_theta = parameter[5];
		x_index = parameter[6];
		y_index = parameter[7];
		z_index = parameter[8];	
	};

	////
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
    
	////0305:保存mirror旋转平移参数
	double tran_x = 0 ,tran_y = 0 ,tran_z = 0 ;
	double rotate_x_theta = 0 ,rotate_y_theta = 0 ,rotate_z_theta = 0 ;
	double x_index = 1 ,y_index = 2 ,z_index = 3 ;
};
