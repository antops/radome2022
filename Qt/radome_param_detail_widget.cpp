#include "radome_param_detail_widget.h"

#include "../def.h"

RadomeParamDatailWidget::RadomeParamDatailWidget(QWidget *parent)
	: QDialog(parent)
{
	lable_ = new QLabel(QString::fromLocal8Bit("材料:"));
	combo_box_ = new QComboBox();
	ok_bt_ = new QPushButton(QString::fromLocal8Bit("确认"));
	connect(ok_bt_, SIGNAL(clicked()), this, SLOT(OnOKClicked()));

	QGridLayout * layout = new QGridLayout;
	layout->addWidget(lable_, 0, 0);
	layout->addWidget(combo_box_, 0, 1);
	param_box_ = new QGroupBox(QString::fromLocal8Bit("材料选择"));
	param_box_->setLayout(layout);

	QHBoxLayout * hlayout = new QHBoxLayout;
	hlayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hlayout->addWidget(ok_bt_);

	QVBoxLayout * Vlayout = new QVBoxLayout(this);
	Vlayout->addWidget(param_box_);
	Vlayout->addLayout(hlayout);
}

RadomeParamDatailWidget::~RadomeParamDatailWidget()
{
}

void RadomeParamDatailWidget::SetParam(int index, int mat_id, const std::map<int, MaterialData*>& material_book)
{
	combox_2_mat_index_.clear();
	mat_id_ = mat_id;
	index_ = index;
	combo_box_->clear();
	for (int i = 0; i <= combo_box_->count(); i++) {
		combo_box_->removeItem(i);
	}
	
	int num = 1;
	combox_2_mat_index_[0] = -1;
	combo_box_->addItem(QString::fromLocal8Bit("未定义"));
	int select_id = 0;
	for (const auto& x : material_book) {
		auto name = x.second->GetName();
		combo_box_->addItem(QString::fromLocal8Bit(name.c_str()));
		combox_2_mat_index_.emplace(num, x.first);
		if (mat_id_ == x.first) select_id = num;
		num++;
	}
	combo_box_->setCurrentIndex(select_id);
}

void RadomeParamDatailWidget::OnOKClicked() {
	mat_id_ = combox_2_mat_index_[combo_box_->currentIndex()];
	emit UpdateRotateRadomeParam(index_);
}

int RadomeParamDatailWidget::GetParam() {
	return mat_id_;
}