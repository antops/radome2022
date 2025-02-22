#include "material_param_detail_widget.h"

#include "../def.h"

static int index = 1;

MaterialParamDatailWidget::MaterialParamDatailWidget(bool is_del, QWidget *parent)
	: QDialog(parent)
{
	name_lable_ = new QLabel(QString::fromLocal8Bit("材料名:"));
	name_edit_ = new QLineEdit(QString::fromLocal8Bit("自定义介质_") + QString::number(index));

	eps_lable_ = new QLabel(QString::fromLocal8Bit("eps:"));
	eps_edit_ = new QLineEdit(QString::number(1.0));

	mu_lable_ = new QLabel(QString::fromLocal8Bit("mu:"));
	mu_edit_ = new QLineEdit(QString::number(1.0));

	loss_lable_ = new QLabel(QString::fromLocal8Bit("lossT:"));
	loss_edit_ = new QLineEdit(QString::number(0.001));

	ok_bt_ = new QPushButton(QString::fromLocal8Bit("确认"));
	connect(ok_bt_, SIGNAL(clicked()), this, SLOT(OnOKClicked()));
	if (is_del) {
		del_bt_ = new QPushButton(QString::fromLocal8Bit("删除"));
		connect(del_bt_, SIGNAL(clicked()), this, SLOT(OnDelClicked()));
	}


	QGridLayout * layout = new QGridLayout;
	layout->addWidget(name_lable_, 0, 0);
	layout->addWidget(name_edit_, 0, 1);
	layout->addWidget(eps_lable_, 1, 0);
	layout->addWidget(eps_edit_, 1, 1);
	layout->addWidget(mu_lable_, 2, 0);
	layout->addWidget(mu_edit_, 2, 1);
	layout->addWidget(loss_lable_, 3, 0);
	layout->addWidget(loss_edit_, 3, 1);
	param_box_ = new QGroupBox(QString::fromLocal8Bit("电磁参数"));
	param_box_->setLayout(layout);

	QHBoxLayout * hlayout = new QHBoxLayout;
	hlayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hlayout->addWidget(ok_bt_);
	if (is_del) {
		hlayout->addWidget(del_bt_);
	}
	QVBoxLayout * Vlayout = new QVBoxLayout(this);
	Vlayout->addWidget(param_box_);
	Vlayout->addLayout(hlayout);
}

MaterialParamDatailWidget::~MaterialParamDatailWidget()
{
}

void MaterialParamDatailWidget::SetParam(const MaterialData& data, int index)
{
	eps_ = data.GetEps0();
	mu_ = data.GetMu0();
	lossT_ = data.GetLossT();
	name_edit_->setText(QString::fromLocal8Bit(data.GetName().c_str()));
	eps_edit_->setText(QString::number(eps_));
	mu_edit_->setText(QString::number(mu_));
	loss_edit_->setText(QString::number(lossT_));
	index_ = index;
}

void MaterialParamDatailWidget::OnOKClicked() {

	PARSE_EDIT_LINE_TO_DOUBLE(eps_edit_, eps_);
	PARSE_EDIT_LINE_TO_DOUBLE(mu_edit_, mu_);
	PARSE_EDIT_LINE_TO_DOUBLE(loss_edit_, lossT_);
	data_.SetName(std::string(name_edit_->text().toLocal8Bit()));
	data_.SetMaterialData(eps_, mu_, lossT_);
	accept();
	index++;
	emit UpdateMaterialRadomeParam(index_);
}

void MaterialParamDatailWidget::OnDelClicked() {
	emit DelMaterialRadomeParam(index_);
}