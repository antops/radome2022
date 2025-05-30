#include "fdtd_calc_config_widget.h"

#include <vtkjsoncpp/json/json.h>

#include "../def.h"

FDTDCalcConfigWidget::FDTDCalcConfigWidget(double fre_, QWidget *parent)
	: QDialog(parent)
{
	fre_lable_ = new QLabel(QString::fromLocal8Bit("频率(GHz):"));
	fre_edit_ = new QLineEdit(QString::number(fre_ / 1e9));

	mesh_type_lable_ = new QLabel(QString::fromLocal8Bit("剖分精度:"));
	mesh_type_combobox_ = new QComboBox;
	mesh_type_combobox_->addItem(QString::fromLocal8Bit("1/20波长"));
	mesh_type_combobox_->addItem(QString::fromLocal8Bit("1/25波长"));
	mesh_type_combobox_->addItem(QString::fromLocal8Bit("1/30波长"));
	mesh_type_combobox_->addItem(QString::fromLocal8Bit("1/40波长"));

	is_calc_nonrodome_lable_ = new QLabel(QString::fromLocal8Bit("是否计算无罩:"));
	
	switch_group_button_ = new QButtonGroup();
	yes_unit_ = new QRadioButton(QString::fromLocal8Bit("是"));
	no_unit_ = new QRadioButton(QString::fromLocal8Bit("否"));
	switch_group_button_->addButton(yes_unit_, 0);
	switch_group_button_->addButton(no_unit_, 1);
	yes_unit_->setChecked(true);
	no_unit_->setChecked(false);

	ok_bt_ = new QPushButton(QString::fromLocal8Bit("确认"));
	connect(ok_bt_, SIGNAL(clicked()), this, SLOT(OnOKClicked()));
	

	QGridLayout * layout = new QGridLayout;
	//layout->addWidget(fre_lable_, 0, 0);
	//layout->addWidget(fre_edit_, 0, 1);
	layout->addWidget(mesh_type_lable_, 1, 0);
	layout->addWidget(mesh_type_combobox_, 1, 1);
	// layout->addWidget(is_calc_nonrodome_lable_, 2, 0);
	//layout->addWidget(yes_unit_, 2, 1);
	//layout->addWidget(no_unit_, 2, 2);
	param_box_ = new QGroupBox(QString::fromLocal8Bit("仿真配置"));
	param_box_->setLayout(layout);

	QHBoxLayout * hlayout = new QHBoxLayout;
	hlayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hlayout->addWidget(ok_bt_);

	QVBoxLayout * Vlayout = new QVBoxLayout(this);
	Vlayout->addWidget(param_box_);
	Vlayout->addLayout(hlayout);
}



void FDTDCalcConfigWidget::OnOKClicked() {

	PARSE_EDIT_LINE_TO_DOUBLE(fre_edit_, fre_);
	is_calc_nonrodome_ = (switch_group_button_->checkedId() == 0);
	mesh_type_ = mesh_type_combobox_->currentIndex();
	accept();
}

