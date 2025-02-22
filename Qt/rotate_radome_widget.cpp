#include "rotate_radome_widget.h"

#include <fstream>
#include <QStringList>
#include <QTextStream>
#include <QFile>

#include <vtkjsoncpp/json/json.h>
#include "../RadomeDef/rotate_param/rotate_line_base.h"
#include "../def.h"

// 颜色池
std::vector<QColor> COLOR_LINE_POOL{
	QColor(255, 110, 40), 
	QColor(40, 110, 255),
	QColor(255, 0, 255)
};

RotateRadomeWidget::RotateRadomeWidget(QWidget *parent)
	: QDialog(parent)
{
	InitCustomPlot();
	InitLineTreeItem();
	InitScaleParam();
	InitLineParam();

	okbtn_ = new QPushButton(QString::fromLocal8Bit("生成天线罩"));
	connect(okbtn_, SIGNAL(clicked()), this, SLOT(OnButtonOK()));
	okbtn_->setMaximumHeight(30);
	okbtn_->setMaximumWidth(100);

	save_btn_ = new QPushButton(QString::fromLocal8Bit("保存参数"));
	connect(save_btn_, SIGNAL(clicked()), this, SLOT(OnButtonSave()));
	save_btn_->setMaximumHeight(30);
	save_btn_->setMaximumWidth(100);
	load_btn_ = new QPushButton(QString::fromLocal8Bit("读取参数"));
	connect(load_btn_, SIGNAL(clicked()), this, SLOT(OnButtonLoad()));
	load_btn_->setMaximumHeight(30);
	load_btn_->setMaximumWidth(100);

	// 布局
	QHBoxLayout * hlayout_btn = new QHBoxLayout;
	hlayout_btn->addWidget(add_line_btn_);
	hlayout_btn->addWidget(del_line_btn_);

	QVBoxLayout * vlayout = new QVBoxLayout;
	vlayout->addWidget(tree_widget_);
	vlayout->addLayout(hlayout_btn);

	QVBoxLayout * vlayout2 = new QVBoxLayout;
	vlayout2->addWidget(scale_box_);
	vlayout2->addWidget(line_param_box_);

	QHBoxLayout * hlayout = new QHBoxLayout;
	hlayout->addLayout(vlayout);
	hlayout->addLayout(vlayout2);

	QHBoxLayout * hlayout_btn2 = new QHBoxLayout;
	hlayout_btn2->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hlayout_btn2->addWidget(save_btn_);
	hlayout_btn2->addWidget(load_btn_);
	hlayout_btn2->addWidget(okbtn_);

	QVBoxLayout * mainlayout = new QVBoxLayout(this);
	mainlayout->addWidget(custom_plot_);
	mainlayout->addLayout(hlayout);
	mainlayout->addLayout(hlayout_btn2);
}

RotateRadomeWidget::~RotateRadomeWidget()
{
}

std::vector<RotateLineBase*> RotateRadomeWidget::GetLineData() {
	std::vector<RotateLineBase*> lines;
	for (auto x : line_data_map_) {
		lines.push_back(x.second);
	}
	return lines;
}

void RotateRadomeWidget::InitCustomPlot() {
	custom_plot_ = new QCustomPlot;
	custom_plot_->xAxis->setLabel("z");
	custom_plot_->yAxis->setLabel("x");
	// 设置背景色
	//PlotCurve->setBackground(QColor(50, 50, 50));
	custom_plot_->xAxis->setRange(0, 100);
	custom_plot_->yAxis->setRange(0, 0.1);
	//custom_plot_->setMaximumHeight(200);
	//custom_plot_->setMaximumWidth(400);
}

void RotateRadomeWidget::InitLineTreeItem() {
	tree_widget_ = new QTreeWidget();
	tree_widget_->setMaximumHeight(300);
	tree_widget_->setMaximumWidth(400);
	tree_widget_->setMinimumWidth(400);
	tree_widget_->setHeaderHidden(true);  // 隐藏表头
	//connect(tree_widget_, SIGNAL(customContextMenuRequested(QPoint)),
	//	this, SLOT(on_treeWidget_ContextMenuRequested(QPoint)));
	connect(tree_widget_, SIGNAL(itemPressed(QTreeWidgetItem*, int)),
		this, SLOT(OntreeWidgetLeftPressed(QTreeWidgetItem*, int)));
	line_tree_item_ = new QTreeWidgetItem(tree_widget_, QStringList(QString::fromLocal8Bit("曲线")));

	add_line_btn_ = new QPushButton(QString::fromLocal8Bit("添加"));
	del_line_btn_ = new QPushButton(QString::fromLocal8Bit("删除"));
	connect(add_line_btn_, SIGNAL(clicked()), this, SLOT(OnButtonAdd()));
	connect(del_line_btn_, SIGNAL(clicked()), this, SLOT(OnButtonDel()));
}

void RotateRadomeWidget::InitScaleParam()
{
	x_scale_lable_ = new QLabel(QString::fromLocal8Bit("x_scale"));
	x_scale_edit_ = new QLineEdit("1.0");
	y_scale_lable_ = new QLabel(QString::fromLocal8Bit("y_scale"));
	y_scale_edit_ = new QLineEdit("1.0");
	QGridLayout * layout = new QGridLayout;
	layout->addWidget(x_scale_lable_, 0, 0);
	layout->addWidget(x_scale_edit_, 0, 1);
	layout->addWidget(y_scale_lable_, 1, 0);
	layout->addWidget(y_scale_edit_, 1, 1);
	scale_box_ = new QGroupBox(QString::fromLocal8Bit("缩放比例"));
	scale_box_->setMaximumHeight(90);
	scale_box_->setLayout(layout);
}

void RotateRadomeWidget::InitLineParam() {
	line_param_box_ = new QGroupBox(QString::fromLocal8Bit("曲线参数"));
	line_sel_name_label_ = new QLabel(QString::fromLocal8Bit("选中的曲线"));
	line_name_label_ = new QLabel();
	QHBoxLayout * hlayout_name = new QHBoxLayout;
	hlayout_name->addWidget(line_sel_name_label_);
	hlayout_name->addWidget(line_name_label_);

	undo_btn_ = new QPushButton(QString::fromLocal8Bit("还原"));
	show_btn_ = new QPushButton(QString::fromLocal8Bit("画图"));
	conform_btn_ = new QPushButton(QString::fromLocal8Bit("应用"));
	connect(undo_btn_, SIGNAL(clicked()), this, SLOT(OnButtonUndo()));
	connect(show_btn_, SIGNAL(clicked()), this, SLOT(OnButtonShow()));
	connect(conform_btn_, SIGNAL(clicked()), this, SLOT(OnButtonConform()));
	QHBoxLayout * hlayout2 = new QHBoxLayout;
	hlayout2->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hlayout2->addWidget(undo_btn_);
	hlayout2->addWidget(show_btn_);
	hlayout2->addWidget(conform_btn_);

	// 直线参数 
	y_lable_ = new QLabel("y = ");
	b_edit_ = new QLineEdit("1");
	x_lable_ = new QLabel("*x +");
	c_edit_ = new QLineEdit("1");
	QHBoxLayout * hlayout_xy = new QHBoxLayout;
	hlayout_xy->addWidget(y_lable_);
	hlayout_xy->addWidget(b_edit_);
	hlayout_xy->addWidget(x_lable_);
	hlayout_xy->addWidget(c_edit_);
	range_lable_ = new QLabel("range: ");
	range_min_edit_ = new QLineEdit("0");
	range_lable_2_ = new QLabel("----");
	range_max_edit_ = new QLineEdit("1");
	QHBoxLayout * hlayout_range = new QHBoxLayout;
	hlayout_range->addWidget(range_lable_);
	hlayout_range->addWidget(range_min_edit_);
	hlayout_range->addWidget(range_lable_2_);
	hlayout_range->addWidget(range_max_edit_);
	QVBoxLayout * vlayout_param = new QVBoxLayout();
	vlayout_param->addLayout(hlayout_xy);
	vlayout_param->addLayout(hlayout_range);

	// 二次曲线参数 
	conic_y_lable_ = new QLabel("y = ");
	conic_a_edit_ = new QLineEdit("1");
	conic_x2_lable_ = new QLabel("* x^2 + ");
	conic_b_edit_ = new QLineEdit("1");
	conic_x_lable_ = new QLabel("*x +");
	conic_c_edit_ = new QLineEdit("1");
	QHBoxLayout * conic_hlayout_xy = new QHBoxLayout;
	conic_hlayout_xy->addWidget(conic_y_lable_);
	conic_hlayout_xy->addWidget(conic_a_edit_);
	conic_hlayout_xy->addWidget(conic_x2_lable_);
	conic_hlayout_xy->addWidget(conic_b_edit_);
	conic_hlayout_xy->addWidget(conic_x_lable_);
	conic_hlayout_xy->addWidget(conic_c_edit_);
	conic_range_lable_ = new QLabel("range: ");
	conic_range_min_edit_ = new QLineEdit("0");
	conic_range_lable_2_ = new QLabel("----");
	conic_range_max_edit_ = new QLineEdit("1");
	QHBoxLayout * conic_hlayout_range = new QHBoxLayout;
	conic_hlayout_range->addWidget(conic_range_lable_);
	conic_hlayout_range->addWidget(conic_range_min_edit_);
	conic_hlayout_range->addWidget(conic_range_lable_2_);
	conic_hlayout_range->addWidget(conic_range_max_edit_);
	QVBoxLayout * conic_vlayout_param = new QVBoxLayout();
	conic_vlayout_param->addLayout(conic_hlayout_xy);
	conic_vlayout_param->addLayout(conic_hlayout_range);

	x0_lable_ = new QLabel("x0 = ");
	x0_edit_ = new QLineEdit("0");
	theta_lable_ = new QLabel("theta = ");
	theta_edit_ = new QLineEdit("0");
	r1_lable_ = new QLabel("r = ");
	r1_edit_ = new QLineEdit("0");
	r2_lable_ = new QLabel("h = ");
	r2_edit_ = new QLineEdit("0");
	QGridLayout* layout_circular = new QGridLayout();
	layout_circular->addWidget(x0_lable_, 0, 0);
	layout_circular->addWidget(x0_edit_, 0, 1);
	layout_circular->addWidget(theta_lable_, 0, 2);
	layout_circular->addWidget(theta_edit_, 0, 3);
	layout_circular->addWidget(r1_lable_, 1, 0);
	layout_circular->addWidget(r1_edit_, 1, 1);
	layout_circular->addWidget(r2_lable_, 1, 2);
	layout_circular->addWidget(r2_edit_, 1, 3);

	// 冯卡门
	fen_a_lable_ = new QLabel("a = ");
	fen_a_edit_ = new QLineEdit("0");
	fen_b_lable_ = new QLabel("b = ");
	fen_b_edit_ = new QLineEdit("0.1016");
	fen_c_lable_ = new QLabel("c = ");
	fen_c_edit_ = new QLineEdit("0.568");
	fen_x_lable_ = new QLabel("len = ");
	fen_x_edit_ = new QLineEdit("0.567");
	QGridLayout* layout_fen = new QGridLayout();
	layout_fen->addWidget(fen_a_lable_, 0, 0);
	layout_fen->addWidget(fen_a_edit_, 0, 1);
	layout_fen->addWidget(fen_b_lable_, 0, 2);
	layout_fen->addWidget(fen_b_edit_, 0, 3);
	layout_fen->addWidget(fen_c_lable_, 1, 0);
	layout_fen->addWidget(fen_c_edit_, 1, 1);
	layout_fen->addWidget(fen_x_lable_, 1, 2);
	layout_fen->addWidget(fen_x_edit_, 1, 3);

	// 冯卡门倒角
	fen_r_a_lable_ = new QLabel("a = ");
	fen_r_a_edit_ = new QLineEdit("0");
	fen_r_b_lable_ = new QLabel("b = ");
	fen_r_b_edit_ = new QLineEdit("0.1016");
	fen_r_c_lable_ = new QLabel("c = ");
	fen_r_c_edit_ = new QLineEdit("0.568");
	fen_r_r_lable_ = new QLabel("r = ");
	fen_r_r_edit_ = new QLineEdit("0.01");
	fen_r_x_lable_ = new QLabel("len = ");
	fen_r_x_edit_ = new QLineEdit("0.567");
	QGridLayout* layout_fen_r = new QGridLayout();
	layout_fen_r->addWidget(fen_r_a_lable_, 0, 0);
	layout_fen_r->addWidget(fen_r_a_edit_, 0, 1);
	layout_fen_r->addWidget(fen_r_b_lable_, 0, 2);
	layout_fen_r->addWidget(fen_r_b_edit_, 0, 3);
	layout_fen_r->addWidget(fen_r_c_lable_, 1, 0);
	layout_fen_r->addWidget(fen_r_c_edit_, 1, 1);
	layout_fen_r->addWidget(fen_r_r_lable_, 1, 2);
	layout_fen_r->addWidget(fen_r_r_edit_, 1, 3);
	layout_fen_r->addWidget(fen_r_x_lable_, 2, 0);
	layout_fen_r->addWidget(fen_r_x_edit_, 2, 1);

	import_lable_ = new QLabel(QString::fromLocal8Bit("导入曲线地址, 每行一组数据, xy以空格分隔:"));
	import_edit_ = new QLineEdit("");
	import_bt_ = new QPushButton(QString::fromLocal8Bit("导入..."));
	QGridLayout* layout_import = new QGridLayout();
	layout_import->addWidget(import_lable_, 0, 0);
	layout_import->addWidget(import_edit_, 1, 0);
	layout_import->addWidget(import_bt_, 1, 1);

	connect(import_bt_, SIGNAL(clicked()), this, SLOT(OnImportBt()));


	line_para_widget_ = new QWidget;
	line_para_widget_->setLayout(vlayout_param);

	line_para_widget2_ = new QWidget;
	line_para_widget2_->setLayout(conic_vlayout_param);

	line_para_widget3_ = new QWidget;
	line_para_widget3_->setLayout(layout_circular);

	line_para_widget4_ = new QWidget;
	line_para_widget4_->setLayout(layout_fen);

	line_para_widget5_ = new QWidget;
	line_para_widget5_->setLayout(layout_fen_r);

	line_para_widget6_ = new QWidget;
	line_para_widget6_->setLayout(layout_import);

	tab_widget_ = new QTabWidget();
	tab_widget_->setMaximumHeight(200);
	tab_widget_->addTab(line_para_widget_, QString::fromLocal8Bit("直线"));
	tab_widget_->addTab(line_para_widget2_, QString::fromLocal8Bit("二次曲线"));
	tab_widget_->addTab(line_para_widget3_, QString::fromLocal8Bit("圆头直线"));
	tab_widget_->addTab(line_para_widget4_, QString::fromLocal8Bit("冯卡门曲线"));
	tab_widget_->addTab(line_para_widget5_, QString::fromLocal8Bit("冯卡门倒角曲线"));
	tab_widget_->addTab(line_para_widget6_, QString::fromLocal8Bit("导入离散点"));
	tab_widget_->setCurrentIndex(4);

	connect(tab_widget_, SIGNAL(currentChanged(int)), this, SLOT(OnLeftPressedLineTypeBoxChanged(int)));

	sampling_label_ = new QLabel("sampling: ");
	sampling_edit_ = new QLineEdit("100");

	cut_label_ = new QLabel(QString::fromLocal8Bit("截断"));
	cut_edit_ = new QLineEdit("-1");
	QHBoxLayout * hlayout_sampling = new QHBoxLayout;
	hlayout_sampling->addWidget(sampling_label_);
	hlayout_sampling->addWidget(sampling_edit_);
	hlayout_sampling->addWidget(cut_label_);
	hlayout_sampling->addWidget(cut_edit_);

	QVBoxLayout * mainlayout = new QVBoxLayout();
	mainlayout->addLayout(hlayout_name);
	//mainlayout->addLayout(hlayout);
	mainlayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	mainlayout->addWidget(tab_widget_);
	mainlayout->addLayout(hlayout_sampling);
	mainlayout->addLayout(hlayout2);

	line_param_box_->setLayout(mainlayout);
	line_param_box_->setMaximumHeight(300);
	CreateStraightLine(-1);
}

void setEqualPixelScales(QCustomPlot* customPlot) {
	// 获取图表区域的宽度和高度  
	int width = customPlot->width();
	int height = customPlot->height();

	// 假设你已经有了一些数据，并且图表已经根据这些数据进行了适当的缩放  
	// 这里我们使用当前的数据范围来计算比例因子  
	QCPRange xRange = customPlot->xAxis->range();
	QCPRange yRange = customPlot->yAxis->range();

	// 计算比例因子，使得 X 轴和 Y 轴的像素格大小尽可能接近  
	// 这里我们简单地使用图表区域的宽度和高度作为基准  
	double min = std::min(xRange.lower, yRange.lower);
	double max = std::max(xRange.upper, yRange.upper);

	// 调整轴的比例，使得它们的像素格大小尽可能相等  
	// customPlot->xAxis->scaleRange(xRange.lower * scaleFactor / xScale, xRange.upper * scaleFactor / xScale);
	customPlot->yAxis->setRange(-max / width * height / 2, max / width * height / 2);
	customPlot->xAxis->setRange(min, max);

	// 重新绘制图表以反映更改  
	customPlot->replot();
}

void RotateRadomeWidget::UpdatePlot() {
	custom_plot_->clearGraphs();
	int index = 0;

	double max_x = 0.0;

	for (auto pair : line_data_map_) {
		const std::vector<Vector3>& lines = pair.second->GetLines();
		if (lines.empty()) continue;
		custom_plot_->addGraph();
		if (index < COLOR_LINE_POOL.size()) {
			custom_plot_->graph(2*index)->setPen(QPen(COLOR_LINE_POOL[index]));
		}
		QVector<double>tz, tx;
		tz.push_back(lines[0].z);
		tx.push_back(0.0);
		for (const auto& v3 : lines) {
			tz.push_back(v3.z);
			tx.push_back(v3.x);
			if (v3.x > max_x) max_x = v3.x;
		}
		custom_plot_->graph(2 * index)->setData(tz, tx);

		custom_plot_->addGraph();
		QVector<double>tz1, tx1;
		tz1.push_back(lines[0].z);
		tx1.push_back(0.0);
		for (const auto& v3 : lines) {
			tz1.push_back(v3.z);
			tx1.push_back(-v3.x);
		}
		if (index < COLOR_LINE_POOL.size()) {
			custom_plot_->graph(2 * index + 1)->setPen(QPen(COLOR_LINE_POOL[index]));
		}
		custom_plot_->graph(2 * index + 1)->setData(tz1, tx1);
		index++;
	}
	if (cut_x_ > 0.0) {
		QVector<double>tz_x, tx_x;
		tz_x.push_back(cut_x_);
		tz_x.push_back(cut_x_);
		tx_x.push_back(max_x);
		tx_x.push_back(-max_x);
		custom_plot_->addGraph();
		custom_plot_->graph(2 * index)->setData(tz_x, tx_x);
		custom_plot_->graph(2 * index)->setPen(QColor(127, 0, 128));
	}

	custom_plot_->rescaleAxes();
	//custom_plot_->xAxis->setRange(min_z, max_z*1.2);
	//custom_plot_->yAxis->setRange(min_x, max_x*1.2);
	// setEqualPixelScales(custom_plot_);
	custom_plot_->replot();
}


void RotateRadomeWidget::OnButtonAdd() {
	QTreeWidgetItem * tree = new QTreeWidgetItem(QStringList
	(QString("Line ") + QString::number(line_num_++)));
	line_tree_item_->addChild(tree);
	line_name_label_->setText(tree->text(0));
	tree->setData(0, Qt::UserRole, QVariant(RotateLineBase::karman_line_radius_type)); // 默认直线
	tree->setData(1, Qt::UserRole, QVariant(line_index_++));

	tree->setSelected(true);
	line_tree_item_->setExpanded(true);
	select_tree_ = tree;
	tab_widget_->setCurrentIndex(4);
}

void RotateRadomeWidget::OnButtonDel() {
	line_tree_item_->removeChild(select_tree_);
}

void RotateRadomeWidget::OntreeWidgetLeftPressed(QTreeWidgetItem* item, int) {
	if (item->parent() == NULL) return; // 根节点
	if (qApp->mouseButtons() == Qt::LeftButton)
	{
		select_tree_ = item;
		line_name_label_->setText(item->text(0));
		//RectangleWidget dialog(this, isSet); 
		if (item->data(0, Qt::UserRole) == RotateLineBase::straight_line_type)
		{
			CreateStraightLine(item->data(1, Qt::UserRole).toInt());
			tab_widget_->setCurrentIndex(0);
		}
		else if (item->data(0, Qt::UserRole) == RotateLineBase::conic_line_type)
		{
			CreateConicLine(item->data(1, Qt::UserRole).toInt());
			tab_widget_->setCurrentIndex(1);
		}
		else if (item->data(0, Qt::UserRole) == RotateLineBase::circular_straight_line_type)
		{
			CreateCircularStraightLine(item->data(1, Qt::UserRole).toInt());
			tab_widget_->setCurrentIndex(2);
		}
		else if (item->data(0, Qt::UserRole) == RotateLineBase::karman_line_type)
		{
			CreateKarmanLine(item->data(1, Qt::UserRole).toInt());
			tab_widget_->setCurrentIndex(3);
		}
		else if (item->data(0, Qt::UserRole) == RotateLineBase::karman_line_radius_type)
		{
			CreateKarmanRadiusLine(item->data(1, Qt::UserRole).toInt());
			tab_widget_->setCurrentIndex(4);
		}
		else if (item->data(0, Qt::UserRole) == RotateLineBase::import_point_type)
		{
			CreateImportLine(item->data(1, Qt::UserRole).toInt());
			tab_widget_->setCurrentIndex(5);
		}
	}

}

void RotateRadomeWidget::CreateStraightLine(int number) {
	sampling_edit_->setEnabled(true);
	auto it = line_data_map_.find(number);
	if (it == line_data_map_.end()) return;
	if (it->second == NULL) return;
	const std::vector<double>& param = it->second->GetParam();
	b_edit_->setText(QString::number(param[1]));
	c_edit_->setText(QString::number(param[2]));
	range_min_edit_->setText(QString::number(param[3]));
	range_max_edit_->setText(QString::number(param[4]));
	sampling_edit_->setText(QString::number(param[5]));
}

void RotateRadomeWidget::CreateConicLine(int number) {
	sampling_edit_->setEnabled(true);
	auto it = line_data_map_.find(number);
	if (it == line_data_map_.end()) return;
	if (it->second == NULL) return;
	const std::vector<double>& param = it->second->GetParam();
	conic_a_edit_->setText(QString::number(param[0]));
	conic_b_edit_->setText(QString::number(param[1]));
	conic_c_edit_->setText(QString::number(param[2]));
	conic_range_min_edit_->setText(QString::number(param[3]));
	conic_range_max_edit_->setText(QString::number(param[4]));
	sampling_edit_->setText(QString::number(param[5]));
}

void RotateRadomeWidget::CreateCircularStraightLine(int number)
{
	sampling_edit_->setEnabled(true);
	auto it = line_data_map_.find(number);
	if (it == line_data_map_.end()) return;
	if (it->second == NULL) return;
	const std::vector<double>& param = it->second->GetParam();
	x0_edit_->setText(QString::number(param[0]));
	theta_edit_->setText(QString::number(param[1]));
	r1_edit_->setText(QString::number(param[2]));
	r2_edit_->setText(QString::number(param[3]));
	sampling_edit_->setText(QString::number(param[4]));
}

void RotateRadomeWidget::CreateKarmanLine(int number)
{
	sampling_edit_->setEnabled(true);
	auto it = line_data_map_.find(number);
	if (it == line_data_map_.end()) return;
	if (it->second == NULL) return;


	const std::vector<double>& param = it->second->GetParam();
	fen_a_edit_->setText(QString::number(param[1]));
	fen_b_edit_->setText(QString::number(param[2]));
	fen_c_edit_->setText(QString::number(param[3]));
	fen_x_edit_->setText(QString::number(param[4]));
	sampling_edit_->setText(QString::number(param[5]));
	
}

void RotateRadomeWidget::CreateKarmanRadiusLine(int number)
{
	sampling_edit_->setEnabled(true);
	auto it = line_data_map_.find(number);
	if (it == line_data_map_.end()) return;
	if (it->second == NULL) return;


	const std::vector<double>& param = it->second->GetParam();
	fen_r_a_edit_->setText(QString::number(param[1]));
	fen_r_b_edit_->setText(QString::number(param[2]));
	fen_r_c_edit_->setText(QString::number(param[3]));
	fen_r_x_edit_->setText(QString::number(param[4]));
	sampling_edit_->setText(QString::number(param[5]));
	fen_r_r_edit_->setText(QString::number(param[6]));
}

void RotateRadomeWidget::CreateImportLine(int number)
{
	sampling_edit_->setEnabled(false);
	auto it = line_data_map_.find(number);
	if (it == line_data_map_.end()) return;
	if (it->second == NULL) return;
}

void RotateRadomeWidget::OnLeftPressedLineTypeBoxChanged(int index) {
	if (index == 0) {
		CreateStraightLine(-1);
	} else if (index == 1) {
		CreateConicLine(-1);
	} else if (index == 2) {
		CreateCircularStraightLine(-1);
	} else if (index == 3) {
		CreateKarmanLine(-1);
	} else if (index == 4) {
		CreateKarmanRadiusLine(-1);
	} else if (index == 5) {
		CreateImportLine(-1);
	}
}

void RotateRadomeWidget::OnButtonShow() {
	UpdatePlot();
}

void RotateRadomeWidget::OnButtonConform() {
	if (select_tree_ == NULL) {
		QMessageBox::warning(NULL, "Error", QString::fromLocal8Bit("请先添加line"));
		return;
	}
	std::vector<double> param(7);

	int index = select_tree_->data(1, Qt::UserRole).toInt();
	int type_index = tab_widget_->currentIndex();
	RotateLineBase* line_data = NULL; 
	if (type_index == 0) {
		line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::straight_line_type);
		param[0] = 0.0;
		PARSE_EDIT_LINE_TO_DOUBLE(b_edit_, param[1]);
		PARSE_EDIT_LINE_TO_DOUBLE(c_edit_, param[2]);
		PARSE_EDIT_LINE_TO_DOUBLE(range_min_edit_, param[3]);
		PARSE_EDIT_LINE_TO_DOUBLE(range_max_edit_, param[4]);
		PARSE_EDIT_LINE_TO_DOUBLE(sampling_edit_, param[5]);
		select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::straight_line_type));
	}
	else if (type_index == 1) {
		line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::conic_line_type);
		PARSE_EDIT_LINE_TO_DOUBLE(conic_a_edit_, param[0]);
		PARSE_EDIT_LINE_TO_DOUBLE(conic_b_edit_, param[1]);
		PARSE_EDIT_LINE_TO_DOUBLE(conic_c_edit_, param[2]);
		PARSE_EDIT_LINE_TO_DOUBLE(conic_range_min_edit_, param[3]);
		PARSE_EDIT_LINE_TO_DOUBLE(conic_range_max_edit_, param[4]);
		PARSE_EDIT_LINE_TO_DOUBLE(sampling_edit_, param[5]);
		select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::conic_line_type));
	}
	else if (type_index == 2) {
		line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::circular_straight_line_type);
		PARSE_EDIT_LINE_TO_DOUBLE(x0_edit_, param[0]);
		PARSE_EDIT_LINE_TO_DOUBLE(theta_edit_, param[1]);
		PARSE_EDIT_LINE_TO_DOUBLE(r1_edit_, param[2]);
		PARSE_EDIT_LINE_TO_DOUBLE(r2_edit_, param[3]);
		PARSE_EDIT_LINE_TO_DOUBLE(sampling_edit_, param[4]);
		select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::circular_straight_line_type));
	}
	else if (type_index == 3) {
		line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::karman_line_type);
		// TODO
		PARSE_EDIT_LINE_TO_DOUBLE(fen_a_edit_, param[1]);
		PARSE_EDIT_LINE_TO_DOUBLE(fen_b_edit_, param[2]);
		PARSE_EDIT_LINE_TO_DOUBLE(fen_c_edit_, param[3]);
		PARSE_EDIT_LINE_TO_DOUBLE(fen_x_edit_, param[4]);
		PARSE_EDIT_LINE_TO_DOUBLE(sampling_edit_, param[5]);
		select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::karman_line_type));
	}
	else if (type_index == 4) {
		line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::karman_line_radius_type);
		// TODO
		PARSE_EDIT_LINE_TO_DOUBLE(fen_r_a_edit_, param[1]);
		PARSE_EDIT_LINE_TO_DOUBLE(fen_r_b_edit_, param[2]);
		PARSE_EDIT_LINE_TO_DOUBLE(fen_r_c_edit_, param[3]);
		PARSE_EDIT_LINE_TO_DOUBLE(fen_r_x_edit_, param[4]);
		PARSE_EDIT_LINE_TO_DOUBLE(sampling_edit_, param[5]);
		PARSE_EDIT_LINE_TO_DOUBLE(fen_r_r_edit_, param[6]);

		select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::karman_line_radius_type));
	}
	else if (type_index == 5) {
		line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::import_point_type);
		if (!ReadData(&param)) {
			return;
		}

		select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::import_point_type));
	}
	auto it = line_data_map_.find(index);
	if (it != line_data_map_.end()) {
		if (it->second) {
			delete it->second;
		}
	}
	line_data_map_[index] = line_data;

	line_data->SetLineParamAndUpdate(param);

	conic_a_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	conic_b_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	conic_c_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	conic_range_min_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	conic_range_max_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	b_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	c_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	range_min_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	range_max_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	sampling_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	x0_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	theta_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	r1_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	r2_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");

	PARSE_EDIT_LINE_TO_DOUBLE(cut_edit_, cut_x_);
	cut_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
	
}

void RotateRadomeWidget::OnButtonOK() {
	PARSE_EDIT_LINE_TO_DOUBLE(x_scale_edit_, x_scale_);
	PARSE_EDIT_LINE_TO_DOUBLE(y_scale_edit_, y_scale_);
	emit UpdateRotateRadome();
}

void RotateRadomeWidget::OnButtonSave() {
	QString filename = QFileDialog::getSaveFileName(this,
		tr("Save the file"),
		"",
		tr("*.json"));
	if (filename.isEmpty()) {
		return;
	}
	
	Json::Value js;
	js["version"] = 1;
	Json::Value& js_content = js["content"];
	int index = 0;
	for (auto& pair : line_data_map_) {
		Json::Value& value = js_content[index];
		const std::vector<double>& param = pair.second->GetParam();
		for (const auto& x : param) {
			value["param"].append(x);
		}
		value["type"] = (int)(pair.second->GetLineType());
		index++;
	}
	js["cut_x"] = cut_x_;
	std::string js_dir = filename.toStdString();
	std::ofstream outfile(js_dir);
	if (!outfile.is_open())
	{
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("打开文件失败"));
		return;
	}
	outfile << js.toStyledString();

	outfile.close();
}

void RotateRadomeWidget::OnButtonLoad() {
	QString filename = QFileDialog::getOpenFileName(this,
		tr("open file"),
		"",
		tr("*.json"));
	if (filename.isEmpty())
	{
		return;
	}
	std::string js_dir = filename.toStdString();
	std::ifstream file(js_dir);
	if (!file.is_open()) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("打开文件失败"));
		return;
	}
	Json::Reader reader;
	Json::Value js;
	if (!reader.parse(file, js))  // reader将Json字符串解析到root，root将包含Json里所有子元素  
	{
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("读取文件失败"));
		return;
	}
	try {
		line_data_map_.clear();
		line_num_ = 0;
		line_index_ = 0;
		if (js.isMember("cut_x")) {
			cut_x_ = js["cut_x"].asDouble();
			cut_edit_->setText(QString::number(cut_x_));
		}
		for (int i = 0; i < js["content"].size(); i++) {
			const Json::Value& value = js["content"][i];
			int line_index = i;
			int type_index = value["type"].asInt();

			QTreeWidgetItem * tree = new QTreeWidgetItem(QStringList
			(QString("Line ") + QString::number(line_num_++)));
			line_index_++;
			line_tree_item_->addChild(tree);
			line_name_label_->setText(tree->text(0));
			tree->setData(0, Qt::UserRole, QVariant(RotateLineBase::straight_line_type)); // 默认直线
			tree->setData(1, Qt::UserRole, QVariant(line_index));

			tree->setSelected(true);
			line_tree_item_->setExpanded(true);
			select_tree_ = tree;

			RotateLineBase* line_data = NULL;
			std::vector<double> param;
			for (int j = 0; j < value["param"].size(); j++) {
				param.push_back(value["param"][j].asDouble());
			}
			if (type_index == 0) {
				line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::straight_line_type);
				select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::straight_line_type));
			}
			else if (type_index == 1) {
				line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::conic_line_type);
				select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::conic_line_type));
			}
			else if (type_index == 2) {
				line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::circular_straight_line_type);
				select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::circular_straight_line_type));
			}
			else if (type_index == 3) {
				line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::karman_line_type);
				select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::karman_line_type));
			}
			else if (type_index == 4) {
				line_data = RotateLineBaseFactory::GenRotateLine(RotateLineBase::karman_line_radius_type);
				select_tree_->setData(0, Qt::UserRole, QVariant(RotateLineBase::karman_line_radius_type));
			}
			else {
				QMessageBox::warning(NULL, "Warning",
					QString::fromLocal8Bit("加载文件失败"));
				return;
			}
			line_data_map_[line_index] = line_data;
			line_data->SetLineParamAndUpdate(param);
		}
	}
	catch (...) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("加载文件失败"));
		return;
	}
	
	UpdatePlot();
}

void RotateRadomeWidget::OnImportBt()
{
	QString filename = QFileDialog::getOpenFileName(this,
		tr("Open the file"),
		"",
		tr("*.txt"));
	if (!filename.isEmpty())
	{
		import_edit_->setText(filename);
	}
	import_edit_->setStyleSheet("background-color:rgba(255,255,255,255)");
}

bool RotateRadomeWidget::ReadData(std::vector<double>* data) {
	QString q_path = import_edit_->text();
	if (q_path.isEmpty())
	{
		import_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("请导入地址"));
		return false;
	}
	QFile file(q_path);
	if (!file.open(QIODevice::ReadOnly |QIODevice::Text)) {
		import_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("打开文件失败"));
		return false;
	}
	data->clear();
	QTextStream in(&file);
	double max_x = 0;
	while (!in.atEnd()) {
		QString line = in.readLine();
		QStringList fields = line.split(" ");
		if (fields.size() < 2) {
			import_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("数据有误:") + line);
			return false;
		}
		
		bool ok,ok1;
		double valuex = fields[0].toDouble(&ok);
		double valuey = fields[1].toDouble(&ok1);
		if (ok&&ok1) {
			if (valuex > valuex) {
				max_x = valuex;
			}
			data->push_back(valuex);
			data->push_back(valuey);
		}
		else {
			
			import_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
			QMessageBox::warning(NULL, "Warning",
				QString::fromLocal8Bit("数据有误:") + line);
			return false;
		}
		
	}
	//for (int i = 0; i < data->size(); i+=2) {
	//	double tmp = data->at(i);
	//	(*data)[i] = max_x - tmp;
	//}
	return true;
}
