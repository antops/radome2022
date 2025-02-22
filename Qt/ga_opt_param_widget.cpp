#include "ga_opt_param_widget.h"

#include "../def.h"


GaOptParamWidget::GaOptParamWidget(int radome_num, QWidget *parent)
	: QDialog(parent)
{
	
	size_pop_lable_ = new QLabel(QString::fromLocal8Bit("每代种群数量:"));
	size_pop_edit_ = new QLineEdit(QString::fromLocal8Bit("200"));

	prob_mut_lable_ = new QLabel(QString::fromLocal8Bit("变异概率:"));
	prob_mut_edit_= new QLineEdit(QString::fromLocal8Bit("0.03"));

	max_iter_lable_ = new QLabel(QString::fromLocal8Bit("最大迭代次数:"));
	max_iter_edit_ = new QLineEdit(QString::fromLocal8Bit("50"));

	tab_widget_ = new QTabWidget;

	model_param_box_ = new QGroupBox(QString::fromLocal8Bit("模型参数设置"));
	QGridLayout* model_layout = new QGridLayout;
	model_layout->addWidget(tab_widget_, 0, 0);
	model_param_box_->setLayout(model_layout);
	

	for (int i = 0; i < radome_num; i++) {
		RadomeGaParamsQt params;
		InitRadomeGaParams(&params, i);
		tab_widget_->addTab(params.param_box_, QString::fromLocal8Bit("天线罩 ") + QString::number(i + 1));
		params_vec_.push_back(params);
	}

	QGridLayout * layout = new QGridLayout;
	layout->addWidget(size_pop_lable_, 0, 0);
	layout->addWidget(size_pop_edit_, 0, 1);
	layout->addWidget(prob_mut_lable_, 1, 0);
	layout->addWidget(prob_mut_edit_, 1, 1);
	layout->addWidget(max_iter_lable_, 2, 0);
	layout->addWidget(max_iter_edit_, 2, 1);
	opt_param_box_ = new QGroupBox(QString::fromLocal8Bit("优化参数设置"));
	opt_param_box_->setLayout(layout);

	ok_bt_ = new QPushButton(QString::fromLocal8Bit("确认"));
	connect(ok_bt_, SIGNAL(clicked()), this, SLOT(OnOKClicked()));
	cancel_bt_ = new QPushButton(QString::fromLocal8Bit("取消"));
	connect(cancel_bt_, SIGNAL(clicked()), this, SLOT(OnDelClicked()));

	QHBoxLayout * hlayout = new QHBoxLayout;
	hlayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hlayout->addWidget(ok_bt_);
	hlayout->addWidget(cancel_bt_);
	
	QVBoxLayout * Vlayout = new QVBoxLayout(this);
	Vlayout->addWidget(model_param_box_);
	Vlayout->addWidget(opt_param_box_);
	Vlayout->addLayout(hlayout);
}

GaOptParamWidget::~GaOptParamWidget()
{
}

void GaOptParamWidget::InitRadomeGaParams(RadomeGaParamsQt* param, int index) {
	param->index_lable_= new QLabel(QString::fromLocal8Bit("天线罩 ") + QString::number(index + 1));
	param->d_lable_ = new QLabel(QString::fromLocal8Bit("厚度取值范围:"));
	param->d_min_edit_ = new QLineEdit(QString::fromLocal8Bit("1.5"));
	param->d_max_edit_ = new QLineEdit(QString::fromLocal8Bit("2.9"));
	param->e_lable_ = new QLabel(QString::fromLocal8Bit("介电常数取值范围:"));
	param->e_min_edit_ = new QLineEdit(QString::fromLocal8Bit("2.5"));
	param->e_max_edit_ = new QLineEdit(QString::fromLocal8Bit("4.3"));
	QGridLayout* layout = new QGridLayout;
	layout->addWidget(param->index_lable_, 0, 0);
	layout->addWidget(param->d_lable_, 1, 0);
	layout->addWidget(param->d_min_edit_, 1, 1);
	layout->addWidget(param->d_max_edit_, 1, 2);
	layout->addWidget(param->e_lable_, 2, 0);
	layout->addWidget(param->e_min_edit_, 2, 1);
	layout->addWidget(param->e_max_edit_, 2, 2);
	param->param_box_ = new QGroupBox(QString::fromLocal8Bit(""));
	param->param_box_->setLayout(layout);
}

bool GaOptParamWidget::RadomeGaParamsQt::ParseRadomeGaParams(double* d_max, double* d_min,
	double* e_min, double* e_max) {
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN_BOOL(d_max_edit_, *d_max);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN_BOOL(d_min_edit_, *d_min);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN_BOOL(e_max_edit_, *e_max);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN_BOOL(e_min_edit_, *e_min);
}

bool GaOptParamWidget::ParseRadomeGaParams() {
	int size_pop = 100;
	PARSE_EDIT_LINE_TO_INT_RETURN(size_pop_edit_, size_pop, 0, 20000);
	param_.size_pop = size_pop;
	double prob_mut = 0.01;
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN_BOOL(prob_mut_edit_, prob_mut, 0.0, 1.0);
	param_.prob_mut = prob_mut;
	int max_iter = 50;
	PARSE_EDIT_LINE_TO_INT_RETURN(max_iter_edit_, max_iter, 0, 10000);
	param_.max_iter = max_iter;
	param_.model_meta_vec.clear();
	for (auto& x: params_vec_) {
		GaParams::GaModelMeta meta;
		if (!x.ParseRadomeGaParams(&meta.d_max, &meta.d_min,
			&meta.e_min, &meta.e_max)) {
			return false;
		}
		param_.model_meta_vec.push_back(meta);
	}
	return true;
}


void GaOptParamWidget::OnOKClicked() {
	if (!ParseRadomeGaParams()) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("参数设置有误"));
		return;
	}

	//PARSE_EDIT_LINE_TO_DOUBLE(eps_edit_, eps_);
	//PARSE_EDIT_LINE_TO_DOUBLE(mu_edit_, mu_);
	//PARSE_EDIT_LINE_TO_DOUBLE(loss_edit_, lossT_);
	//data_.SetName(std::string(name_edit_->text().toLocal8Bit()));
	//data_.SetMaterialData(eps_, mu_, lossT_);
	accept();
	//index++;
	//emit UpdateMaterialRadomeParam(index_);
}

void GaOptParamWidget::OnDelClicked() {
	close();
}