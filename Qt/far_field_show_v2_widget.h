#ifndef FAR_FIELD_SHOW_V2_WIDGET_H
#define FAR_FIELD_SHOW_V2_WIDGET_H


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

#include "../VTK/farfield.h"
#include "qcustomplot.h"
#include "../data_manager.h"

class FarFieldShowV2Widget : public QDialog
{
	Q_OBJECT

public:
	FarFieldShowV2Widget(DataManager* data_manager);
	~FarFieldShowV2Widget();

	void UpdateCombox();
	void SelectMarkerType();

private slots:
	void OnAbOkBtn();
	void OnAddBtn();
	void OnClearBtn();
	void OnComboBoxChange(int index);
	void OnSwitchClickedUnit();
	void OnNormalizeClickedUnit();
	
private:
	struct FieldPlotParam {
		int field_index = 0;
		int cut_index = 0;
		FarField* far_field = nullptr;
		int index = 0;
	};
	void UpdatePlotAB(QCustomPlot* plot, const FieldPlotParam& param);
	void GetValueByXBox(QVector<double>& zhou_vec, FarField* far_field, double* min_v, double* max_v, int index);
	void SelectPoint(QCPAbstractPlottable* plottable, int index, QMouseEvent* event);
    void OnPlotClick(QCPAbstractPlottable* , int , QMouseEvent* );
	void GetValueByYBox(QVector<double>& zhou_vec, FarField* far_field, double* min_v, double* max_v, int cut_inde);
private:
	// basic info
	QLabel* x_label_;
	QComboBox* x_a_;
	QLabel* y_label_;
	QComboBox* y_a_;
	QLabel* point_label_;	
    QComboBox* point_combo_box_;

    QGroupBox* Marker_box_;
	QCPItemText* text_tip_11;
	QCPItemText* text_tip_22;
	QGroupBox* switch_box_;
	QButtonGroup* switch_group_button_unit_;
	QRadioButton* linear_button_unit_;
	QRadioButton* dB_button_unit_;
	QLabel* normalize_label_;
	QButtonGroup* normalize_button_unit_;
	QRadioButton* normalize_yes_unit_;
	QRadioButton* normalize_no_unit_;

	bool is_dB_ = true;
	bool is_normalize_ = true;

	QGroupBox* basic_info_;

	// ab ���
	QGroupBox* ab_group_box_;
	QComboBox* combo_box_a_;
	QLabel* a_label_;
	QComboBox* combo_box_cut_;
	QLabel* cut_label_;
	QPushButton* add_btn_;
	QPushButton* clear_btn_;

	QListWidget* list_widget_;



	QPushButton* ab_ok_btn_;
	// ����
	QCustomPlot* custom_plot_;
	QGroupBox* plot_box_;



	std::vector<FarField*> select_far_field_;
	std::vector<FieldPlotParam> plot_param_vec_;


	DataManager* data_manager_;
	std::map<int, int> data_index_map_;
	bool is_ok_ = false;


	double x_max_ = 0.0;
	double y_max_ = 0.0;
	double x_min_ = 0.0;
	double y_min_ = 0.0;
};

#endif // FAR_FIELD_SHOW_V2_WIDGET_H