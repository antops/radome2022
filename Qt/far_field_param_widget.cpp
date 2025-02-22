#include "far_field_param_widget.h"

#include "../def.h"

FarFieldParamWidget::FarFieldParamWidget(bool is_del, QWidget *parent)
	: QDialog(parent)
{
	switch_group_button_unit_ = new QButtonGroup();
	linear_button_unit_ = new QRadioButton(QString::fromLocal8Bit("线性单元方向图"));
	dB_button_unit_ = new QRadioButton(QString::fromLocal8Bit("dB单元方向图"));
	switch_group_button_unit_->addButton(linear_button_unit_, 0);
	switch_group_button_unit_->addButton(dB_button_unit_, 1);
	linear_button_unit_->setChecked(false);
	dB_button_unit_->setChecked(true);
	connect(linear_button_unit_, SIGNAL(clicked()), this, SLOT(OnSwitchClickedUnit()));
	connect(dB_button_unit_, SIGNAL(clicked()), this, SLOT(OnSwitchClickedUnit()));

	max_label_ = new QLabel("Max:");
	max_label_->setMaximumWidth(50);
	max_show_label_ = new QLabel("0");
	max_show_label_->setMaximumWidth(50);
	min_label_ = new QLabel("Min:");
	min_label_->setMaximumWidth(50);
	min_show_label_ = new QLabel("-40");
	min_show_label_->setMaximumWidth(50);

	max_scroll_bar_ = new QScrollBar(Qt::Horizontal);
	max_scroll_bar_->setRange(-40, 0);
	max_scroll_bar_->setSingleStep(1);
	max_scroll_bar_->setValue(20);
	min_scroll_bar_ = new QScrollBar(Qt::Horizontal);
	min_scroll_bar_->setRange(-40, 0);
	min_scroll_bar_->setSingleStep(1);
	min_scroll_bar_->setValue(-30);

	QHBoxLayout* maxLayoutdB = new QHBoxLayout;
	maxLayoutdB->addWidget(max_label_);
	maxLayoutdB->addWidget(max_show_label_);
	maxLayoutdB->addWidget(max_scroll_bar_);
	QHBoxLayout* minLayoutdB = new QHBoxLayout;
	minLayoutdB->addWidget(min_label_);
	minLayoutdB->addWidget(min_show_label_);
	minLayoutdB->addWidget(min_scroll_bar_);

	QHBoxLayout * SwitchLayoutLinear = new QHBoxLayout;
	SwitchLayoutLinear->addWidget(linear_button_unit_, 0, 0);


	QPushButton* update_btn_ = new QPushButton(QString::fromLocal8Bit("更新"));
	connect(update_btn_, SIGNAL(clicked()), this, SLOT(OnButtonUpdate()));

	QVBoxLayout* SwitchLayout = new QVBoxLayout(this);
	SwitchLayout->addLayout(SwitchLayoutLinear);
	SwitchLayout->addWidget(dB_button_unit_);
	SwitchLayout->addLayout(maxLayoutdB);
	SwitchLayout->addLayout(minLayoutdB);
	SwitchLayout->addWidget(update_btn_);

	connect(max_scroll_bar_, SIGNAL(valueChanged(int)), this, SLOT(MaxChanged(int)));
	connect(min_scroll_bar_, SIGNAL(valueChanged(int)), this, SLOT(MinChanged(int)));

	//QVBoxLayout *layout = new QVBoxLayout;
}

FarFieldParamWidget::~FarFieldParamWidget()
{
}


void FarFieldParamWidget::MaxChanged(int val)
{
	//max = val;
	max_show_label_->setText(QString::number(val));
	param_.max = val;
}

void FarFieldParamWidget::MinChanged(int val)
{
	//min = val;
	min_show_label_->setText(QString::number(val));
	param_.min = val;
}


void FarFieldParamWidget::OnSwitchClickedUnit() {
	int x = switch_group_button_unit_->checkedId();
	if (x == 0)//选择线性显示单元方向图
	{
		param_.is_dB_unit = false;
		min_label_->setVisible(false);
		min_show_label_->setVisible(false);
		min_scroll_bar_->setVisible(false);
		max_label_->setVisible(false);
		max_show_label_->setVisible(false);
		max_scroll_bar_->setVisible(false);
	}
	else
	{
		param_.is_dB_unit = true;
		min_label_->setVisible(true);
		min_show_label_->setVisible(true);
		min_scroll_bar_->setVisible(true);
		max_label_->setVisible(true);
		max_show_label_->setVisible(true);
		max_scroll_bar_->setVisible(true);

	}
}

void FarFieldParamWidget::OnButtonUpdate() {
	emit UpdateFarField();
}