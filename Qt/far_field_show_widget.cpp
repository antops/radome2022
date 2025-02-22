#include "far_field_show_widget.h"
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

FarFieldShowWidget::FarFieldShowWidget(DataManager* data_manager) :
	data_manager_(data_manager)
{
	//resize(1000, 800);

	a_label_ = new QLabel(QString::fromLocal8Bit("结果A:"));
	b_label_ = new QLabel(QString::fromLocal8Bit("结果B:"));
	a_label_->setMaximumWidth(50);
	b_label_->setMaximumWidth(50);
	combo_box_a_ = new QComboBox;
	combo_box_b_ = new QComboBox;
	ab_ok_btn_ = new QPushButton(QString::fromLocal8Bit("确定"));
	ab_ok_btn_->setMaximumWidth(150);
	connect(ab_ok_btn_, SIGNAL(clicked()), this, SLOT(OnAbOkBtn()));
	QGridLayout * ab_layout = new QGridLayout;
	ab_layout->addWidget(a_label_, 0, 0);
	ab_layout->addWidget(combo_box_a_, 0, 1);
	ab_layout->addWidget(b_label_, 1, 0);
	ab_layout->addWidget(combo_box_b_, 1, 1);
	ab_layout->addWidget(ab_ok_btn_, 1, 2);

	ab_group_box_ = new QGroupBox;
	ab_group_box_->setTitle(QString::fromLocal8Bit("选择所需对比的计算结果"));
	ab_group_box_->setLayout(ab_layout);
	ab_group_box_->setMaximumHeight(150);

	UpdateCombox();

	custom_plot_ = new QCustomPlot;
	custom_plot_->xAxis->setLabel("theta");
	custom_plot_->yAxis->setLabel("E");
	custom_plot_->setMinimumHeight(250);
	custom_plot_->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom);
	connect(custom_plot_, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)),
		this, SLOT(OnPlotClick(QCPAbstractPlottable*, int, QMouseEvent*))); //关联选点信号
	text_tip_ = new QCPItemText(custom_plot_);
	text_tip_->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
	text_tip_->position->setType(QCPItemPosition::ptAbsolute);
	QFont font;
	font.setPixelSize(20);
	text_tip_->setFont(font); // make font a bit larger
	text_tip_->setPen(QPen(Qt::black)); // show black border around text
	text_tip_->setBrush(Qt::white);
	text_tip_->setVisible(false);

	phi_label_ = new QLabel("phi:");
	phi_label_->setMaximumWidth(50);
	phi_label_->setMaximumHeight(30);
	phi_show_label_ = new QLabel("0");
	phi_show_label_->setMaximumWidth(50);
	phi_show_label_->setMaximumHeight(30);
	phi_scroll_bar_ = new QScrollBar(Qt::Vertical);

	QVBoxLayout* phi_layout = new QVBoxLayout;
	phi_layout->addWidget(phi_label_);
	phi_layout->addWidget(phi_scroll_bar_);
	phi_layout->addWidget(phi_show_label_);

	QHBoxLayout * hlayout1 = new QHBoxLayout;
	hlayout1->addWidget(custom_plot_);
	hlayout1->addLayout(phi_layout);

	plot_box_ = new QGroupBox;
	plot_box_->setTitle(QString::fromLocal8Bit("AB结果对比图(固定phi,theta方向)"));
	plot_box_->setLayout(hlayout1);
	
	custom_plot2_ = new QCustomPlot;
	custom_plot2_->xAxis->setLabel("phi");
	custom_plot2_->yAxis->setLabel("E");
	custom_plot2_->setMinimumHeight(250);
	custom_plot2_->setInteractions(QCP::iSelectPlottables | QCP::iRangeDrag | QCP::iRangeZoom);
	connect(custom_plot2_, SIGNAL(plottableClick(QCPAbstractPlottable*, int, QMouseEvent*)),
		this, SLOT(OnPlotClick2(QCPAbstractPlottable*, int, QMouseEvent*))); //关联选点信号
	text_tip2_ = new QCPItemText(custom_plot2_);
	text_tip2_->setPositionAlignment(Qt::AlignTop | Qt::AlignHCenter);
	text_tip2_->position->setType(QCPItemPosition::ptAbsolute);

	text_tip2_->setFont(font); // make font a bit larger
	text_tip2_->setPen(QPen(Qt::black)); // show black border around text
	text_tip2_->setBrush(Qt::white);
	text_tip2_->setVisible(false);

	theta_label_ = new QLabel("theta:");
	theta_label_->setMaximumHeight(30);
	theta_label_->setMaximumWidth(50);
	theta_show_label_ = new QLabel("0");
	theta_show_label_->setMaximumHeight(30);
	theta_show_label_->setMaximumWidth(50);
	theta_scroll_bar_ = new QScrollBar(Qt::Vertical);

	QVBoxLayout* thetaLayoutdB = new QVBoxLayout;
	thetaLayoutdB->addWidget(theta_label_);
	thetaLayoutdB->addWidget(theta_scroll_bar_);
	thetaLayoutdB->addWidget(theta_show_label_);

	QHBoxLayout * hlayout2 = new QHBoxLayout;
	hlayout2->addWidget(custom_plot2_);
	hlayout2->addLayout(thetaLayoutdB);

	plot2_box_ = new QGroupBox;
	plot2_box_->setTitle(QString::fromLocal8Bit("AB结果对比图(固定theta,phi方向)"));
	plot2_box_->setLayout(hlayout2);

	max_gain_label_a_ = new QLabel(QString::fromLocal8Bit("A 最大增益:"));
	max_gain_label_b_ = new QLabel(QString::fromLocal8Bit("B 最大增益:"));
	max_gain_val_a_ = new QLabel("0");
	max_gain_val_b_ = new QLabel("0");
	transmissivity_label_ = new QLabel(QString::fromLocal8Bit("透波率:"));
	transmissivity_val_ = new QLabel("0");
	QGridLayout * gain_layout = new QGridLayout;
	//gain_layout->addWidget(max_gain_label_a_, 0, 0);
	//gain_layout->addWidget(max_gain_val_a_, 0, 1);
	//gain_layout->addWidget(max_gain_label_b_, 1, 0);
	//gain_layout->addWidget(max_gain_val_b_, 1, 1);
	gain_layout->addWidget(transmissivity_label_, 2, 0);
	gain_layout->addWidget(transmissivity_val_, 2, 1);
	gain_box_ = new QGroupBox(QString::fromLocal8Bit("和方向图:"));
	gain_box_->setLayout(gain_layout);

	max_zero_label_a_ = new QLabel(QString::fromLocal8Bit("A 0深位置:"));
	max_zero_label_b_ = new QLabel(QString::fromLocal8Bit("B 0深位置:"));
	max_zero_val_a_ = new QLabel("0");
	max_zero_val_b_ = new QLabel("0");
	aiming_error_label_ = new QLabel(QString::fromLocal8Bit("瞄准误差:"));
	aiming_error_val_ = new QLabel("0");
	QGridLayout * zero_layout = new QGridLayout;
	zero_layout->addWidget(max_zero_label_a_, 0, 0);
	zero_layout->addWidget(max_zero_val_a_, 0, 1);
	zero_layout->addWidget(max_zero_label_b_, 1, 0);
	zero_layout->addWidget(max_zero_val_b_, 1, 1);
	zero_layout->addWidget(aiming_error_label_, 2, 0);
	zero_layout->addWidget(aiming_error_val_, 2, 1);
	zero_box_ = new QGroupBox(QString::fromLocal8Bit("差方向图:"));
	zero_box_->setLayout(zero_layout);

	ab_calc_box_ = new QGroupBox(QString::fromLocal8Bit("AB综合结果对比:"));
	QGridLayout * ab_calc_layout = new QGridLayout;
	ab_calc_layout->addWidget(gain_box_, 0, 0);
	//ab_calc_layout->addWidget(zero_box_, 0, 1);
	ab_calc_box_->setLayout(ab_calc_layout);

	custom_plot_a_ = new QCustomPlot;
	custom_plot_b_ = new QCustomPlot;

	pic_label_a_ = new QLabel;
	QHBoxLayout * plot_a_layout = new QHBoxLayout;
	plot_a_layout->addWidget(pic_label_a_);
	plot_a_layout->addWidget(custom_plot_a_);

	pic_label_b_ = new QLabel;
	QHBoxLayout * plot_b_layout = new QHBoxLayout;
	plot_b_layout->addWidget(custom_plot_b_);
	plot_b_layout->addWidget(pic_label_b_);

	plot_box_a_ = new QGroupBox;
	plot_box_a_->setTitle(QString::fromLocal8Bit("A结果"));
	plot_box_a_->setLayout(plot_a_layout);

	plot_box_b_ = new QGroupBox;
	plot_box_b_->setTitle(QString::fromLocal8Bit("B结果"));
	plot_box_b_->setLayout(plot_b_layout);

	QHBoxLayout * hlayout = new QHBoxLayout;
	hlayout->addWidget(plot_box_a_);
	hlayout->addWidget(plot_box_b_);

	switch_group_button_unit_ = new QButtonGroup();
	linear_button_unit_ = new QRadioButton(QString::fromLocal8Bit("线性单元方向图"));
	dB_button_unit_ = new QRadioButton(QString::fromLocal8Bit("dB单元方向图"));
	switch_group_button_unit_->addButton(linear_button_unit_, 0);
	switch_group_button_unit_->addButton(dB_button_unit_, 1);
	linear_button_unit_->setChecked(false);
	dB_button_unit_->setChecked(true);
	connect(linear_button_unit_, SIGNAL(clicked()), this, SLOT(OnSwitchClickedUnit()));
	connect(dB_button_unit_, SIGNAL(clicked()), this, SLOT(OnSwitchClickedUnit()));

	QHBoxLayout * hlayout_group = new QHBoxLayout;
	hlayout_group->addWidget(linear_button_unit_);
	hlayout_group->addWidget(dB_button_unit_);

	switch_box_ = new QGroupBox;
	switch_box_->setTitle(QString::fromLocal8Bit("显示选项"));
	switch_box_->setLayout(hlayout_group);
	switch_box_->setMaximumHeight(100);

	QVBoxLayout * mainlayout = new QVBoxLayout(this);
	mainlayout->addWidget(ab_group_box_);
	mainlayout->addLayout(hlayout);
	mainlayout->addWidget(plot_box_);
	mainlayout->addWidget(plot2_box_);
	// TODO 
	mainlayout->addWidget(ab_calc_box_); 
	mainlayout->addWidget(switch_box_);

	connect(phi_scroll_bar_, SIGNAL(valueChanged(int)), this, SLOT(UpdatePlotPhi(int)));
	connect(theta_scroll_bar_, SIGNAL(valueChanged(int)), this, SLOT(UpdatePlotTheta(int)));
}

FarFieldShowWidget::~FarFieldShowWidget()
{
}

void FarFieldShowWidget::SetFarField(FarField* far_field_a, FarField* far_field_b) {
	if (far_field_a_ == far_field_a && far_field_b_ == far_field_b) {
		return;
	}
	far_field_a_ = far_field_a;
	far_field_b_ = far_field_b;

	const std::vector<double>& theta = far_field_a_->GetTheta();
	const std::vector<double>& phi = far_field_a_->GetPhi();

	for (const auto& data : theta) {
		x_theta_.push_back(data / 3.14 * 180);
	}
	for (const auto& data : phi) {
		x_phi_.push_back(data / 3.14 * 180);
	}

	phi_scroll_bar_->setRange(0, phi.size() - 1);
	phi_scroll_bar_->setSingleStep(1);
	phi_scroll_bar_->setValue(0);

	theta_scroll_bar_->setRange(0, theta.size() - 1);
	theta_scroll_bar_->setSingleStep(1);
	theta_scroll_bar_->setValue(0);

	UpdatePlotPhi(0);
	UpdatePlotTheta(0);

	if (far_field_a_->GetPicFilePath().empty() || !GlobalConfig::Instance()->IsPlotByPython()) {
		pic_label_a_->setHidden(true);
		custom_plot_a_->setHidden(false);
		UpdatePlotAB(custom_plot_a_, far_field_a_);
	}
	else {
		pic_label_a_->setHidden(false);
		custom_plot_a_->setHidden(true);
		QImage* img_a = new QImage;
		if (!(img_a->load(far_field_a_->GetPicFilePath().c_str()))) //加载图像
		{
			/*QMessageBox::information(this,
				QString::fromLocal8Bit("生成图像失败"),
				QString::fromLocal8Bit("生成图像失败!"));*/
			qDebug((std::string("far field open failed:") + far_field_a_->GetPicFilePath()).c_str());
			delete img_a;
		}
		else {
			pic_label_a_->setPixmap(QPixmap::fromImage(*img_a));
		}
	}

	if (far_field_a_->GetPicFilePath().empty() || !GlobalConfig::Instance()->IsPlotByPython()) {
		pic_label_b_->setHidden(true);
		custom_plot_b_->setHidden(false);
		UpdatePlotAB(custom_plot_b_, far_field_b_);
	}
	else {
		pic_label_b_->setHidden(true);
		custom_plot_b_->setHidden(false);
		QImage* img_b = new QImage;
		if (!(img_b->load(far_field_b_->GetPicFilePath().c_str()))) //加载图像
		{
			/*QMessageBox::information(this,
				QString::fromLocal8Bit("生成图像失败"),
				QString::fromLocal8Bit("生成图像失败!"));*/
			qDebug((std::string("far field open failed:") + far_field_a_->GetPicFilePath()).c_str());
			delete img_b;
		}
		else {
			pic_label_b_->setPixmap(QPixmap::fromImage(*img_b));
		}
	}

	if (!far_field_b_->IsCalcTBL()) {
		auto calc_helper = CalcTbl(far_field_a_, far_field_b_);
		if (!calc_helper.Run()) {
			QMessageBox::information(this,
				QString::fromLocal8Bit("计算透波率失败"),
				QString::fromLocal8Bit("计算透波率失败!"));
		}
	}
	transmissivity_val_->setText(QString::number(far_field_b_->GetTBL()));
}


void FarFieldShowWidget::UpdatePlotAB(QCustomPlot* plot, FarField* far_field)
{
	if (!is_ok_)return;
	const std::vector<std::vector<double>>* E_ptr = nullptr;
	if (is_dB_) {
		E_ptr = &far_field->GetEDB();
		// E_ptr = &far_field->GetEd();
	}
	else {
		E_ptr = &far_field->GetE();
	}
	const std::vector<std::vector<double>>& E = *E_ptr;
	int nx = E.size(), ny = E[0].size();
	plot->clearGraphs();
	
	plot->plotLayout()->remove(plot->plotLayout()->element(0,1));

	// 设置彩虹图颜色映射
	QCPColorMap *colorMap = new QCPColorMap(plot->xAxis, plot->yAxis);

	colorMap->data()->setSize(nx, ny);
	colorMap->data()->setRange(QCPRange(0, nx), QCPRange(0, ny));
	double min = 0;
	double max = 0;
	for (int x = 0; x < nx; ++x)
	{
		for (int y = 0; y < ny; ++y)
		{
			colorMap->data()->setCell(x, y, E[x][y]);
			if (E[x][y] > max) max = E[x][y];
			if (E[x][y] < min) min = E[x][y];
		}
	}


	colorMap->setGradient(QCPColorGradient::gpJet);
	colorMap->rescaleDataRange(true);
	// 设置彩虹图的颜色缩放轴
	QCPColorScale *colorScale = new QCPColorScale(plot);
	plot->plotLayout()->addElement(0, 1, colorScale);
	colorScale->setType(QCPAxis::atRight);
	colorScale->setDataRange(QCPRange(min, max));
	colorScale->setGradient(QCPColorGradient::gpJet);
	colorMap->setColorScale(colorScale);
	colorScale->axis()->setLabel("Value");

	// 显示窗口
	plot->rescaleAxes();
	plot->replot();
}

void FarFieldShowWidget::UpdatePlotAB(QCustomPlot* plot)
{
	const std::vector<std::vector<double>>* E_ptr_a = nullptr;
	if (is_dB_) {
		E_ptr_a = &far_field_a_->GetEDB();
	}
	else {
		E_ptr_a = &far_field_a_->GetE();
	}
	const std::vector<std::vector<double>>& E_a = *E_ptr_a;
	plot->clearGraphs();

	// 设置彩虹图颜色映射
	QCPColorMap *colorMap = new QCPColorMap(plot->xAxis, plot->yAxis);
	int nx = 100, ny = 100;
	colorMap->data()->setSize(nx, ny);
	colorMap->data()->setRange(QCPRange(1, nx), QCPRange(1, ny));
	for (int x = 1; x <= nx; ++x)
	{
		for (int y = 1; y <= ny; ++y)
		{
			colorMap->data()->setCell(x, y, qCos(x / 10.0)*qSin(y / 10.0));
		}
	}

	colorMap->setGradient(QCPColorGradient::gpJet);
	colorMap->rescaleDataRange(true);

	// 设置彩虹图的颜色缩放轴
	QCPColorScale *colorScale = new QCPColorScale(plot);
	plot->plotLayout()->addElement(0, 1, colorScale);
	colorScale->setType(QCPAxis::atRight);
	colorScale->setDataRange(QCPRange(1, 100));
	colorScale->setGradient(QCPColorGradient::gpJet);
	colorMap->setColorScale(colorScale);
	colorScale->axis()->setLabel("Value");

	// 显示窗口
	plot->rescaleAxes();
	plot->replot();
}

void FarFieldShowWidget::UpdatePlotPhi(int index) {
	if (!is_ok_)return;
	const std::vector<std::vector<double>>* E_ptr_a = nullptr;
	const std::vector<std::vector<double>>* E_ptr_b = nullptr;
	if (is_dB_) {
		E_ptr_a = &far_field_a_->GetEDB();
		E_ptr_b = &far_field_b_->GetEDB();
	}
	else {
		E_ptr_a = &far_field_a_->GetE();
		E_ptr_b = &far_field_b_->GetE();
	}
	const std::vector<std::vector<double>>& E_a = *E_ptr_a;
	const std::vector<std::vector<double>>& E_b = *E_ptr_b;
	custom_plot_->clearGraphs();
	double max_y = 0;
	double min_y = 0;
	
	QVector<double> y_a;
	for (int i = 0; i < E_a.size(); ++i) {
		double v = E_a[i][index];
		if (v > max_y) max_y = v;
		if (v < min_y) min_y = v;
		y_a.push_back(v);
	}
	QVector<double> y_b;
	for (int i = 0; i < E_b.size(); ++i) {
		double v = E_b[i][index];
		if (v > max_y) max_y = v;
		if (v < min_y) min_y = v;
		y_b.push_back(v);
	}
	custom_plot_->addGraph();
	custom_plot_->graph(0)->setPen(QPen(QColor(255, 110, 40)));
	custom_plot_->graph(0)->setData(x_theta_, y_a);
	custom_plot_->graph(0)->setName("A");
	custom_plot_->addGraph();
	custom_plot_->graph(1)->setPen(QPen(QColor(40, 110, 255)));
	custom_plot_->graph(1)->setData(x_theta_, y_b);
	custom_plot_->graph(1)->setName("B");
	custom_plot_->xAxis->setRange(x_theta_[0], x_theta_[x_theta_.size()-1]);
	custom_plot_->yAxis->setRange(min_y, max_y*1.2);
	custom_plot_->legend->setVisible(true);
	custom_plot_->replot();

	phi_show_label_->setText(QString::number(x_phi_[index]));
}

void FarFieldShowWidget::UpdatePlotTheta(int index) {
	if (!is_ok_)return;
	const std::vector<std::vector<double>>* E_ptr_a = nullptr;
	const std::vector<std::vector<double>>* E_ptr_b = nullptr;
	if (is_dB_) {
		E_ptr_a = &far_field_a_->GetEDB();
		E_ptr_b = &far_field_b_->GetEDB();
	}
	else {
		E_ptr_a = &far_field_a_->GetE();
		E_ptr_b = &far_field_b_->GetE();
	}
	const std::vector<std::vector<double>>& E_a = *E_ptr_a;
	const std::vector<std::vector<double>>& E_b = *E_ptr_b;

	double max2_y = 1;
	double min2_y = -1;
	QVector<double> y_a;
	for (int i = 0; i < E_a[index].size(); ++i) {
		double v = E_a[index][i];
		if (v > max2_y) max2_y = v;
		if (v < min2_y) min2_y = v;
		y_a.push_back(v);
	}
	QVector<double> y_b;
	for (int i = 0; i < E_b[index].size(); ++i) {
		double v = E_b[index][i];
		if (v > max2_y) max2_y = v;
		if (v < min2_y) min2_y = v;
		y_b.push_back(v);
	}
	custom_plot2_->clearGraphs();
	custom_plot2_->addGraph();
	custom_plot2_->graph(0)->setPen(QPen(QColor(255, 110, 40)));
	custom_plot2_->graph(0)->setData(x_phi_, y_a);
	custom_plot2_->graph(0)->setName("A");
	custom_plot2_->addGraph();
	custom_plot2_->graph(1)->setPen(QPen(QColor(40, 110, 255)));
	custom_plot2_->graph(1)->setData(x_phi_, y_b);
	custom_plot2_->graph(1)->setName("B");
	custom_plot2_->xAxis->setRange(x_phi_[0], x_phi_[x_phi_.size() - 1]);
	custom_plot2_->yAxis->setRange(min2_y, max2_y*1.2);
	custom_plot2_->legend->setVisible(true);
	custom_plot2_->replot();
	theta_show_label_->setText(QString::number(x_theta_[index]));
}

void FarFieldShowWidget::OnSwitchClickedUnit() {
	if (!is_ok_)return;
	int x = switch_group_button_unit_->checkedId();
	FarFieldParam param;
	if (x == 0) {
		is_dB_ = false;
		param.is_dB_unit = false;
	}
	else {
		is_dB_ = true;
		param.is_dB_unit = true;
	}

	far_field_a_->SetFarFieldParam(param);
	if (far_field_a_ != far_field_b_) {
		far_field_b_->SetFarFieldParam(param);
	}

	UpdatePlotPhi(phi_scroll_bar_->value());
	UpdatePlotTheta(theta_scroll_bar_->value());
	//UpdatePlotAB(custom_plot_a_, far_field_a_);
	//UpdatePlotAB(custom_plot_b_, far_field_b_);

}

void FarFieldShowWidget::UpdateCombox() {
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
	func(combo_box_b_, data_manager_);
}

void FarFieldShowWidget::OnPlotClick(QCPAbstractPlottable* plottable, int index, QMouseEvent* event) {
	//先获取点击的绘图层名称,然后通过名称找到图层ID,再找到对应的数据点  这里因为知道ID 所以直接使用 没有通过名称找
	const QCPGraphData *ghd = custom_plot_->graph()->data()->at(index);
	QString text = "(" + QString::number(ghd->key) + "," + QString::number(ghd->value) + ")";
	text_tip_->setText(text);//文本内容填充
	text_tip_->position->setCoords(event->pos().x(), event->pos().y());//文本框所在位置
	text_tip_->setVisible(true);
	custom_plot_->replot();
}

void FarFieldShowWidget::OnPlotClick2(QCPAbstractPlottable* plottable, int index, QMouseEvent* event) {
	//先获取点击的绘图层名称,然后通过名称找到图层ID,再找到对应的数据点  这里因为知道ID 所以直接使用 没有通过名称找
	const QCPGraphData *ghd = custom_plot2_->graph()->data()->at(index);
	QString text = "(" + QString::number(ghd->key) + "," + QString::number(ghd->value) + ")";
	text_tip2_->setText(text);//文本内容填充
	text_tip2_->position->setCoords(event->pos().x(), event->pos().y());//文本框所在位置
	text_tip2_->setVisible(true);
	custom_plot_->replot();
}

void FarFieldShowWidget::OnAbOkBtn() {
	int index_a = combo_box_a_->currentIndex();
	int index_b = combo_box_b_->currentIndex();
	if (index_a < 0) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("未选择对比结果A"));
		return;
	}
	if (index_b < 0) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("未选择对比结果B"));
		return;
	}
	is_ok_ = true;
	int cur_index_a = data_index_map_[combo_box_a_->currentIndex()];
	int cur_index_b = data_index_map_[combo_box_b_->currentIndex()];

	if (data_manager_->GetFarField(cur_index_a)->GetTheta().size()
		!= data_manager_->GetFarField(cur_index_b)->GetTheta().size()
		|| data_manager_->GetFarField(cur_index_a)->GetPhi().size()
		!= data_manager_->GetFarField(cur_index_b)->GetPhi().size()) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("选择的A和B的结果采样不统一，无法进行对比"));
		return;
	}


	SetFarField(data_manager_->GetFarField(cur_index_a), data_manager_->GetFarField(cur_index_b));
}