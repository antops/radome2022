#include "far_field_show_v2_widget.h"
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderWindow.h>
#include <vtkCaptionActor2D.h>
#include <vtkTextProperty.h>
#include <vtkProperty.h>
#include <vtkCamera.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkjsoncpp/json/json.h>
#include "../tools/calc_tbl.h"
#include "../Qt/global_config.h"

std::vector<QColor> COLOR_LINE_POOLV2{
	QColor(255, 110, 40),
	QColor(40, 110, 255),
	QColor(255, 0, 255)
};

FarFieldShowV2Widget::FarFieldShowV2Widget(DataManager* data_manager) :
	data_manager_(data_manager)
{
	x_label_ = new QLabel(QString::fromLocal8Bit("x轴"));
	y_label_ = new QLabel(QString::fromLocal8Bit("y轴"));
	x_label_->setMaximumWidth(50);
	y_label_->setMaximumWidth(50);
	x_a_ = new QComboBox;
	y_a_ = new QComboBox;
	x_a_->addItem(QString::fromLocal8Bit("theta"));
	x_a_->addItem(QString::fromLocal8Bit("phi"));
	connect(x_a_, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxChange(int)));
	// x_a_->addItem(QString::fromLocal8Bit("value"));
	//y_a_->addItem(QString::fromLocal8Bit("theta"));
	//y_a_->addItem(QString::fromLocal8Bit("phi"));
	y_a_->addItem(QString::fromLocal8Bit("value"));
	x_a_->setCurrentIndex(0);
	y_a_->setCurrentIndex(0);

	QHBoxLayout* x_a_layout = new QHBoxLayout;
	x_a_layout->addWidget(x_label_);
	x_a_layout->addWidget(x_a_);
	QHBoxLayout* y_a_layout = new QHBoxLayout;
	y_a_layout->addWidget(y_label_);
	y_a_layout->addWidget(y_a_);


	switch_group_button_unit_ = new QButtonGroup();
	linear_button_unit_ = new QRadioButton(QString::fromLocal8Bit("线性单元方向图"));
	dB_button_unit_ = new QRadioButton(QString::fromLocal8Bit("dB单元方向图"));
	switch_group_button_unit_->addButton(linear_button_unit_, 0);
	switch_group_button_unit_->addButton(dB_button_unit_, 1);
	linear_button_unit_->setChecked(false);
	dB_button_unit_->setChecked(true);

	connect(linear_button_unit_, SIGNAL(clicked()), this, SLOT(OnSwitchClickedUnit()));
	connect(dB_button_unit_, SIGNAL(clicked()), this, SLOT(OnSwitchClickedUnit()));

	QHBoxLayout* unit_layout = new QHBoxLayout;
	unit_layout->addWidget(linear_button_unit_);
	unit_layout->addWidget(dB_button_unit_);


	normalize_button_unit_ = new QButtonGroup();
	normalize_yes_unit_ = new QRadioButton(QString::fromLocal8Bit("归一化"));
	normalize_no_unit_ = new QRadioButton(QString::fromLocal8Bit("原始值"));
	normalize_button_unit_->addButton(normalize_no_unit_, 0);
	normalize_button_unit_->addButton(normalize_yes_unit_, 1);
	normalize_yes_unit_->setChecked(true);
	normalize_no_unit_->setChecked(false);

	connect(normalize_yes_unit_, SIGNAL(clicked()), this, SLOT(OnNormalizeClickedUnit()));
	connect(normalize_no_unit_, SIGNAL(clicked()), this, SLOT(OnNormalizeClickedUnit()));

	QHBoxLayout* normalize_layout = new QHBoxLayout;
	normalize_layout->addWidget(normalize_yes_unit_);
	normalize_layout->addWidget(normalize_no_unit_);

	QVBoxLayout* basic_layout = new QVBoxLayout;
	basic_layout->addLayout(x_a_layout);
	basic_layout->addLayout(y_a_layout);
	basic_layout->addLayout(unit_layout);
	basic_layout->addLayout(normalize_layout);

	switch_box_ = new QGroupBox;
	switch_box_->setTitle(QString::fromLocal8Bit("显示选项"));
	switch_box_->setLayout(basic_layout);
	switch_box_->setMaximumHeight(250);
	switch_box_->setMaximumWidth(400);


	a_label_ = new QLabel(QString::fromLocal8Bit("结果:"));
	a_label_->setMaximumWidth(50);
	combo_box_a_ = new QComboBox;
	connect(combo_box_a_, SIGNAL(currentIndexChanged(int)), this, SLOT(OnComboBoxChange(int)));
	QHBoxLayout* a_labe_layout = new QHBoxLayout;
	a_labe_layout->addWidget(a_label_);
	a_labe_layout->addWidget(combo_box_a_);

	combo_box_cut_ = new QComboBox;
	cut_label_ = new QLabel(QString::fromLocal8Bit("切面:"));
	cut_label_->setMaximumWidth(50);
	QHBoxLayout* cut_layout = new QHBoxLayout;
	cut_layout->addWidget(cut_label_);
	cut_layout->addWidget(combo_box_cut_);
	
	list_widget_ = new QListWidget;
	add_btn_ = new QPushButton(QString::fromLocal8Bit("添加"));
	add_btn_->setMaximumWidth(120);
	connect(add_btn_, SIGNAL(clicked()), this, SLOT(OnAddBtn()));
	clear_btn_ = new QPushButton(QString::fromLocal8Bit("清空"));
	clear_btn_->setMaximumWidth(120);
	connect(clear_btn_, SIGNAL(clicked()), this, SLOT(OnClearBtn()));
	QHBoxLayout* btn_layout = new QHBoxLayout;
	btn_layout->addWidget(add_btn_);
	btn_layout->addWidget(clear_btn_);


	ab_ok_btn_ = new QPushButton(QString::fromLocal8Bit("绘制"));
	ab_ok_btn_->setMaximumWidth(150);
	connect(ab_ok_btn_, SIGNAL(clicked()), this, SLOT(OnAbOkBtn()));
	QVBoxLayout* ab_layout = new QVBoxLayout;
	ab_layout->addLayout(a_labe_layout);
	ab_layout->addLayout(cut_layout);
	ab_layout->addWidget(list_widget_);
	ab_layout->addLayout(btn_layout);

	ab_group_box_ = new QGroupBox;
	ab_group_box_->setTitle(QString::fromLocal8Bit("选择展示的计算结果"));
	ab_group_box_->setLayout(ab_layout);
	ab_group_box_->setMaximumHeight(400);
	ab_group_box_->setMaximumWidth(400);

	UpdateCombox();

	QVBoxLayout* layout_left = new QVBoxLayout();
	layout_left->addWidget(switch_box_);
	layout_left->addWidget(ab_group_box_);
	layout_left->addWidget(ab_ok_btn_);

	custom_plot_ = new QCustomPlot;
	QHBoxLayout* plot_a_layout = new QHBoxLayout;
	plot_a_layout->addWidget(custom_plot_);
	plot_box_ = new QGroupBox;
	plot_box_->setTitle(QString::fromLocal8Bit("结果"));
	plot_box_->setLayout(plot_a_layout);

	QHBoxLayout* layout_main = new QHBoxLayout(this);
	layout_main->addWidget(plot_box_);
	layout_main->addLayout(layout_left);

}

FarFieldShowV2Widget::~FarFieldShowV2Widget()
{
}


void FarFieldShowV2Widget::UpdateCombox() {
	data_index_map_.clear();
	auto all_field = data_manager_->GetAllFarFieldMap();
	int count = 0;
	for (auto& cp : all_field) {
		data_index_map_.emplace(count, cp.first);
		count++;
	}
	auto func = [](QComboBox*combo_box, DataManager* data_manager) {
		int cur_index = combo_box->currentIndex();
		auto all_field = data_manager->GetAllFarFieldMap();
		combo_box->clear();
		for (auto& cp : all_field) {
			combo_box->addItem(QString::fromLocal8Bit(cp.second->GetName().c_str()));
		}
		combo_box->setCurrentIndex(cur_index);
	};
	func(combo_box_a_, data_manager_);
}

void FarFieldShowV2Widget::GetValueByXBox(QVector<double>& zhou_vec, FarField* far_field,
	double* min_v, double* max_v, int index) {
	if (index == 0) {
		const std::vector<double>& theta = far_field->GetTheta();
		for (const auto& data : theta) {
			zhou_vec.push_back(data / 3.14 * 180);
		}
	}
	else if (index == 1) {
		const std::vector<double>& phi = far_field->GetPhi();
		for (const auto& data : phi) {
			zhou_vec.push_back(data / 3.14 * 180);
		}
	}

	if (!zhou_vec.empty()) {
		*min_v = zhou_vec[0];
		*max_v = zhou_vec.back();
		for (auto val : zhou_vec) {
			if (*min_v > val) *min_v = val;
			if (*max_v < val) *max_v = val;
		}
	}
}

void FarFieldShowV2Widget::GetValueByYBox(QVector<double>& zhou_vec, FarField* far_field,
	double* min_v, double* max_v, int cut_index) {
	int index = x_a_->currentIndex();
	const std::vector<std::vector<double>>* E_ptr = nullptr;
	if (is_dB_) {
		if (is_normalize_) {
			E_ptr = &far_field->GetEDB();
		}
		else {
			E_ptr = &far_field->GetEDBOrigin();
		}
		
		// E_ptr = &far_field->GetEd();
	}
	else {
		E_ptr = &far_field->GetE();
	}
	const std::vector<std::vector<double>>& E_a = *E_ptr;
	if (index == 1) {
		for (int i = 0; i < E_a[cut_index].size(); ++i) {
			double v = E_a[cut_index][i];
			zhou_vec.push_back(v);
		}
	}
	else {
		for (int i = 0; i < E_a.size(); ++i) {
			double v = E_a[i][cut_index];
			zhou_vec.push_back(v);
		}
	}

	
	if (!zhou_vec.empty()) {
		*min_v = zhou_vec[0];
		*max_v = zhou_vec.back();
		for (auto val : zhou_vec) {
			if (*min_v > val) *min_v = val;
			if (*max_v < val) *max_v = val;
		}
	}
}


void FarFieldShowV2Widget::UpdatePlotAB(QCustomPlot* plot, const FieldPlotParam& param)
{
	auto* far_field = param.far_field;
	const std::vector<std::vector<double>>* E_ptr = nullptr;
	if (is_dB_) {
		E_ptr = &far_field->GetEDB();
		// E_ptr = &far_field->GetEd();
	}
	else {
		E_ptr = &far_field->GetE();
	}
	QVector<double> x_a_vec;
	QVector<double> y_a_vec;
	double x_max = 0.0;
	double y_max = 0.0;
	double x_min = 0.0;
	double y_min = 0.0;
	GetValueByXBox(x_a_vec, far_field, &x_min, &x_max, x_a_->currentIndex());
	GetValueByYBox(y_a_vec, far_field, &y_min, &y_max, param.cut_index);

	custom_plot_->addGraph();
	int index = custom_plot_->graphCount() - 1;
	auto pen = QPen(COLOR_LINE_POOLV2[index % COLOR_LINE_POOLV2.size()]);
	pen.setWidth(2);
	custom_plot_->graph(index)->setPen(pen);
	custom_plot_->graph(index)->setData(x_a_vec, y_a_vec);
	custom_plot_->graph(index)->setName(QString::fromLocal8Bit("曲线:") + QString::number(index+1));
	custom_plot_->legend->setVisible(true);

	if (index == 0) {
		x_max_ = x_max;
		y_max_ = y_max;
		x_min_ = x_min;
		y_min_ = y_min;
	}
	else {
		if (x_max > x_max_)x_max_ = x_max;
		if (y_max > y_max_)y_max_ = y_max;
		if (x_min_ > x_min)x_min_ = x_min;
		if (y_min_ > y_min)y_min_ = y_min;
	}

	custom_plot_->xAxis->setRange(x_min_, x_max_);
	custom_plot_->yAxis->setRange(y_min_, y_max_);
}

void FarFieldShowV2Widget::OnAbOkBtn() {
	custom_plot_->clearGraphs();
	for (auto& param : plot_param_vec_) {
		UpdatePlotAB(custom_plot_, param);
	}
	custom_plot_->xAxis->setLabel(x_a_->itemText(x_a_->currentIndex()));
	custom_plot_->yAxis->setLabel(y_a_->itemText(y_a_->currentIndex()));
	custom_plot_->replot();
}

void FarFieldShowV2Widget::OnAddBtn() {
	auto all_field = data_manager_->GetAllFarFieldMap();
	FieldPlotParam param;
	param.field_index = combo_box_a_->currentIndex();
	int index = data_index_map_[param.field_index];
	if (all_field.find(index) == all_field.end()) {
		return;
	}
	param.far_field = all_field[index];
	param.cut_index = combo_box_cut_->currentIndex();
	plot_param_vec_.push_back(param);
	list_widget_->addItem(QString::fromLocal8Bit("曲线:") + QString::number(plot_param_vec_.size()) 
		+ QString::fromLocal8Bit("_") + QString::fromLocal8Bit(param.far_field->GetName().c_str())
		+ QString::fromLocal8Bit("_切片位置:") + combo_box_cut_->currentText());

}

void FarFieldShowV2Widget::OnClearBtn() {
	list_widget_->clear();
	plot_param_vec_.clear();
}

void FarFieldShowV2Widget::OnComboBoxChange(int _index) {
	int index = combo_box_a_->currentIndex();
	auto all_field = data_manager_->GetAllFarFieldMap();
	FieldPlotParam param;
	int filed_index = data_index_map_[index];
	if (all_field.find(filed_index) == all_field.end()) {
		return;
	}
	QVector<double> x_a_vec;
	double x_max = 0.0;
	double x_min = 0.0;
	auto* far_filed = all_field[filed_index];
	int tp_index = x_a_->currentIndex();
	if (tp_index == 0) {
		tp_index = 1;
	}
	else {
		tp_index = 0;
	}
	GetValueByXBox(x_a_vec, far_filed, &x_min, &x_max, tp_index);
	combo_box_cut_->clear();
	for (auto x_val : x_a_vec) {
		combo_box_cut_->addItem(QString::number(x_val));
	}
}

void FarFieldShowV2Widget::OnSwitchClickedUnit() {
	int x = switch_group_button_unit_->checkedId();
	if (x == 0) {
		is_dB_ = false;
		normalize_yes_unit_->setHidden(true);
		normalize_no_unit_->setHidden(true);
	}
	else {
		is_dB_ = true;
		normalize_yes_unit_->setHidden(false);
		normalize_no_unit_->setHidden(false);
	}

}

void FarFieldShowV2Widget::OnNormalizeClickedUnit() {
	int x = normalize_button_unit_->checkedId();
	if (x == 0) {
		is_normalize_ = false;
	}
	else {
		is_normalize_ = true;
	}
}