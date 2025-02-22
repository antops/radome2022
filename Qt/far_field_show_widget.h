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

class FarFieldShowWidget : public QDialog
{
	Q_OBJECT

public:
	FarFieldShowWidget(DataManager* data_manager);
	~FarFieldShowWidget();

	void UpdateCombox();
	void SetFarField(FarField* far_field_a, FarField* far_field_b);

private slots:
	void UpdatePlotPhi(int);
	void UpdatePlotTheta(int);
	void OnSwitchClickedUnit();
	void OnPlotClick(QCPAbstractPlottable*, int, QMouseEvent*);
	void OnPlotClick2(QCPAbstractPlottable*, int, QMouseEvent*);
	void OnAbOkBtn();
private:
	void UpdatePlotAB(QCustomPlot* plot);
	void UpdatePlotAB(QCustomPlot* plot, FarField* far_field);

private:
	QGroupBox* plot_box_a_;
	QGroupBox* plot_box_b_;
	QCustomPlot *custom_plot_a_;
	QCustomPlot *custom_plot_b_;
	QLabel* pic_label_a_;
	QLabel* pic_label_b_;

	QGroupBox* plot_box_;
	QCustomPlot *custom_plot_;
	QCPItemText* text_tip_;
	QGroupBox* plot2_box_;
	QCustomPlot *custom_plot2_;
	QCPItemText* text_tip2_;

	FarField* far_field_a_;
	FarField* far_field_b_;
	QVector<double> x_theta_;
	QVector<double> x_phi_;

	QLabel* phi_label_;
	QLabel* phi_show_label_;
	QLabel* theta_label_;
	QLabel* theta_show_label_;

	QScrollBar* phi_scroll_bar_;
	QScrollBar* theta_scroll_bar_;

	QGroupBox* switch_box_;
	QButtonGroup * switch_group_button_unit_;
	QRadioButton * linear_button_unit_;
	QRadioButton * dB_button_unit_;
	bool is_dB_ = true;

	QLabel* a_label_;
	QLabel* b_label_;
	QComboBox* combo_box_a_;
	QComboBox* combo_box_b_;
	QGroupBox* ab_group_box_;
	QPushButton * ab_ok_btn_;

	QLabel* max_gain_label_a_;
	QLabel* max_gain_label_b_;
	QLabel* max_gain_val_a_;
	QLabel* max_gain_val_b_;
	QLabel* transmissivity_label_;
	QLabel* transmissivity_val_;
	QGroupBox* gain_box_;

	QLabel* max_zero_label_a_;
	QLabel* max_zero_label_b_;
	QLabel* max_zero_val_a_;
	QLabel* max_zero_val_b_;
	QLabel* aiming_error_label_;
	QLabel* aiming_error_val_;
	QGroupBox* zero_box_;
	QGroupBox* ab_calc_box_;

	DataManager* data_manager_;
	std::map<int, int> data_index_map_;
	bool is_ok_ = false;
};
