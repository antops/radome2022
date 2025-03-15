#pragma once

#include <map>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QDialog>
#include <QTabWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <Qlabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCloseEvent> 
#include <QVTKWidget.h>

#include <vtkSmartPointer.h>
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <QVTKOpenGLNativeWidget.h> 
#include <vtkAxesActor.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextActor.h>
#include <vtkArrowSource.h>
#include <vtkInteractorStyleTrackballCamera.h>

#include "../VTK/farfield.h"
#include "qcustomplot.h"
#include "../data_manager.h"
#include "../tools/cnpy.h"
#include <fstream>


class FarFieldShowTab : public QDialog
{
	Q_OBJECT

public:
	// FarFieldShowTab(FarField* far_field);
	FarFieldShowTab(DataManager* data_manager);

	~FarFieldShowTab();
	QVTKOpenGLNativeWidget* FarFieldShowTab::Getwidget_();
	void UpdateCombox();


private slots:
	void OnComboBoxChange(int index);
	void OnAbOkBtn();
	void OnfileBtn();
	void OnUnitChange(int);
private:
	void GetValueByYBox(QVector<double>& zhou_vec, FarField* far_field, double* min_v, double* max_v, int cut_inde);
	void GetValueByXBox(QVector<double>& zhou_vec, FarField* far_field, double* min_v, double* max_v);
	
private:
	vtkSmartPointer<vtkRenderWindow> renderWindow;
	////0116:3D
	QVTKOpenGLNativeWidget* widget_;
	vtkSmartPointer<vtkOrientationMarkerWidget> widget1;
	vtkSmartPointer<vtkRenderWindowInteractor> renderWindowInteractor;
	vtkSmartPointer<vtkRenderer> renderer;
	vtkSmartPointer<vtkAxesActor> axes;
	////
	// basic info
	QLabel* x_label_;
	QComboBox* x_a_;
	QLabel* y_label_;
	QComboBox* y_a_;

	QGroupBox* switch_box_;
	QButtonGroup* switch_group_button_unit_;
	QRadioButton* linear_button_unit_;
	QRadioButton* dB_button_unit_;
	bool is_dB_ = true;

	QGroupBox* basic_info_;

	// ab 结果
	QGroupBox* ab_group_box_;
	QComboBox* combo_box_a_;
	QLabel* a_label_;

	QPushButton* ab_ok_btn_;
	// 绘制
	QGroupBox* plot_box_;

	std::vector<FarField*> select_far_field_;

	DataManager* data_manager_;
	std::map<int, int> data_index_map_;
	bool is_ok_ = false;

	double x_max_ = 0.0;
	double y_max_ = 0.0;
	double x_min_ = 0.0;
	double y_min_ = 0.0;

	FarField* far_filed;
	QWidget* layout_main_widget;

	QString fileName;
	QLabel* filePathLabel;

	std::vector<std::vector<double>> E;
	std::vector<std::vector<double>> EDB;
	int file_flag = 0;
};
