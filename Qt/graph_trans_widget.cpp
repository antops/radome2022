#include "graph_trans_widget.h"
#include "../VTK/mirror.h"
#include <QMessageBox>
#include <def.h>

GraphTransWidget::GraphTransWidget(QWidget *parent)
	: QDialog(parent)
{
	transLineEidt = nullptr;
}

GraphTransWidget::~GraphTransWidget()
{

}

void GraphTransWidget::addDefGroupBox(QGroupBox * _defGroupBox, QString filename)
{
	// page1
	//defGroupBox
	QLabel * imgLlabel;
	imgLlabel = new QLabel;
	imgLlabel->setPixmap(QPixmap(filename));

	QGridLayout * layout1 = new QGridLayout;
	layout1->addWidget(imgLlabel);

	//defGroupBox = new QGroupBox;
	_defGroupBox->setLayout(layout1);
	_defGroupBox->setTitle(tr("Definition methods"));
}

void GraphTransWidget::addBaseGroupBox(QGroupBox * _baseGroupBox)
{
	//baseGroupBox
	Ulabel = new QLabel(tr("X"));
	Vlabel = new QLabel(tr("Y"));
	Nlabel = new QLabel(tr("Z"));


	ULineEidt = new QLineEdit;
	VLineEidt = new QLineEdit;
	NLineEidt = new QLineEdit;


	ULineEidt->setText(tr("0.0"));
	VLineEidt->setText(tr("0.0"));
	NLineEidt->setText(tr("0.0"));


	QGridLayout * layout2 = new QGridLayout;
	layout2->addWidget(Ulabel, 0, 0);
	layout2->addWidget(Vlabel, 1, 0);
	layout2->addWidget(Nlabel, 2, 0);
	layout2->addWidget(ULineEidt, 0, 1);
	layout2->addWidget(VLineEidt, 1, 1);
	layout2->addWidget(NLineEidt, 2, 1);


	_baseGroupBox->setTitle(tr("Definition methods"));
	_baseGroupBox->setLayout(layout2);
}

void GraphTransWidget::addTransGroupBox(QGroupBox * _baseGroupBox)
{
	translabel = new QLabel(tr("Power efficiency"));
	transLineEidt = new QLineEdit;
	transLineEidt->setText(tr("1.0"));

	QGridLayout * layout2 = new QGridLayout;
	layout2->addWidget(translabel, 0, 0);
	layout2->addWidget(transLineEidt, 0, 1);

	_baseGroupBox->setTitle(tr("Transmission efficiency"));
	_baseGroupBox->setLayout(layout2);
}
void GraphTransWidget::addRotateGroupBox(QGroupBox* _RotateGroupxyzBox) {
	xthetalabel = new QLabel();
	xthetalabel->setPixmap(QPixmap("Qt/images/x_rotate.png"));
	xthetaLineEidt = new QLineEdit(tr("0.0"));

	ythetalabel = new QLabel();
	ythetalabel->setPixmap(QPixmap("Qt/images/y_rotate.png"));
	ythetaLineEidt = new QLineEdit(tr("0.0"));

	zthetalabel = new QLabel();
	zthetalabel->setPixmap(QPixmap("Qt/images/z_rotate.png"));
	zthetaLineEidt = new QLineEdit(tr("0.0"));

	x_theta_combobox_ = new QComboBox;
	y_theta_combobox_ = new QComboBox;
	z_theta_combobox_ = new QComboBox;

	x_theta_combobox_->addItem(QString::fromLocal8Bit("1"));
	x_theta_combobox_->addItem(QString::fromLocal8Bit("2"));
	x_theta_combobox_->addItem(QString::fromLocal8Bit("3"));

	y_theta_combobox_->addItem(QString::fromLocal8Bit("1"));
	y_theta_combobox_->addItem(QString::fromLocal8Bit("2"));
	y_theta_combobox_->addItem(QString::fromLocal8Bit("3"));

	z_theta_combobox_->addItem(QString::fromLocal8Bit("1"));
	z_theta_combobox_->addItem(QString::fromLocal8Bit("2"));
	z_theta_combobox_->addItem(QString::fromLocal8Bit("3"));

	x_theta_combobox_->setCurrentIndex(0);
	y_theta_combobox_->setCurrentIndex(1);
	z_theta_combobox_->setCurrentIndex(2);

	QLabel* theta_label = new QLabel(QString::fromLocal8Bit("旋转角度"));
	QLabel* theta_num_label = new QLabel(QString::fromLocal8Bit("旋转顺序"));

	QGridLayout* layout8 = new QGridLayout;
	layout8->addWidget(theta_label, 1, 0);
	layout8->addWidget(theta_num_label, 2, 0);
	layout8->addWidget(xthetalabel, 0, 1);
	layout8->addWidget(xthetaLineEidt, 1, 1);
	layout8->addWidget(x_theta_combobox_, 2, 1);
	layout8->addWidget(ythetalabel, 0, 2);
	layout8->addWidget(ythetaLineEidt, 1, 2);
	layout8->addWidget(y_theta_combobox_, 2, 2);
	layout8->addWidget(zthetalabel, 0, 3);
	layout8->addWidget(zthetaLineEidt, 1, 3);
	layout8->addWidget(z_theta_combobox_, 2, 3);

	_RotateGroupxyzBox->setTitle(tr("Rotate workplane"));
	_RotateGroupxyzBox->setLayout(layout8);
}

void GraphTransWidget::addBtn(QGridLayout * _layoutbt, int wayButton)
{

	createbtn = new QPushButton(tr("Ok"));
	closebtn = new QPushButton(tr("Cancel"));

	connect(createbtn, SIGNAL(clicked()), this, SLOT(buttonOk()));
	connect(closebtn, SIGNAL(clicked()), this, SLOT(buttonClose()));

	_layoutbt->addWidget(createbtn, 0, 0);
	_layoutbt->addWidget(closebtn, 0, 2);
}

void GraphTransWidget::buttonClose()
{
	emit sendData(0);
	
}

void GraphTransWidget::buttonOk()
{
	if (!GetGraphTransOk()) {
		return;
	}
	if (transLineEidt)
	{
		double tmp = transLineEidt->text().toDouble();
		dynamic_cast<Mirror*>(mirror)->setTransEffi(tmp);
	}

	emit sendData(1);
}

void GraphTransWidget::on_ComboboxChange(int) {
	on_GraphChange("");
}

void GraphTransWidget::on_GraphChange(QString var)
{
	bool ok = false;
	is_param_ok_ = false;
	double tran_x = ULineEidt->text().toDouble(&ok);
	if (!ok)
	{
		//输出参数有误
		ULineEidt->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}
	double tran_y = VLineEidt->text().toDouble(&ok);
	if (!ok)
	{
		//输出参数有误
		VLineEidt->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}
	double tran_z = NLineEidt->text().toDouble(&ok);
	if (!ok)
	{
		//输出参数有误
		NLineEidt->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}
	double rotate_x_theta = xthetaLineEidt->text().toDouble(&ok);
	if (!ok)
	{
		//输出参数有误
		xthetaLineEidt->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}
	double rotate_y_theta = ythetaLineEidt->text().toDouble(&ok);
	if (!ok)
	{
		//输出参数有误
		ythetaLineEidt->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}
	double rotate_z_theta = zthetaLineEidt->text().toDouble(&ok);
	if (!ok)
	{
		//输出参数有误
		zthetaLineEidt->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}

	std::vector<bool> judge_ok(3, false);
	int x_index = x_theta_combobox_->currentIndex();
	int y_index = y_theta_combobox_->currentIndex();
	int z_index = z_theta_combobox_->currentIndex();
	if (x_index >= 0 && x_index < judge_ok.size()) judge_ok[x_index] = true;
	if (y_index >= 0 && y_index < judge_ok.size()) judge_ok[y_index] = true;
	if (z_index >= 0 && z_index < judge_ok.size()) judge_ok[z_index] = true;

	bool is_theta_num_ok = true;
	for (auto flag : judge_ok) {
		is_theta_num_ok = is_theta_num_ok && flag;
	}
	if (!is_theta_num_ok) {
		x_theta_combobox_->setStyleSheet("background-color:rgba(255,0,0,255)");
		y_theta_combobox_->setStyleSheet("background-color:rgba(255,0,0,255)");
		z_theta_combobox_->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}

	GraphTrans graphTransPara;
	graphTransPara.setGraphTransPar(tran_x, tran_y,
		tran_z, rotate_x_theta, rotate_y_theta, rotate_z_theta,
		x_index, y_index, z_index);

	mirror->setGraphTrans(graphTransPara);
	ULineEidt->setStyleSheet("background-color:rgba(255,255,255,255)");
	VLineEidt->setStyleSheet("background-color:rgba(255,255,255,255)");
	NLineEidt->setStyleSheet("background-color:rgba(255,255,255,255)");
	xthetaLineEidt->setStyleSheet("background-color:rgba(255,255,255,255)");
	ythetaLineEidt->setStyleSheet("background-color:rgba(255,255,255,255)");
	zthetaLineEidt->setStyleSheet("background-color:rgba(255,255,255,255)");

	x_theta_combobox_->setStyleSheet("background-color:rgba(255,255,255,255)");
	y_theta_combobox_->setStyleSheet("background-color:rgba(255,255,255,255)");
	z_theta_combobox_->setStyleSheet("background-color:rgba(255,255,255,255)");

	is_param_ok_ = true;

	emit sendData(2);
}

void GraphTransWidget::setMirror(BasicParameters* mirror)
{
	this->mirror = mirror;

	GraphTrans graphTrans = mirror->getGraphTrans();

	ULineEidt->setText(QString::number(graphTrans.getTrans_x()));
	VLineEidt->setText(QString::number(graphTrans.getTrans_y()));
	NLineEidt->setText(QString::number(graphTrans.getTrans_z()));

	xthetaLineEidt->setText(QString::number(graphTrans.getRotate_x_theta()));
	ythetaLineEidt->setText(QString::number(graphTrans.getRotate_y_theta()));
	zthetaLineEidt->setText(QString::number(graphTrans.getRotate_z_theta()));

	x_theta_combobox_->setCurrentIndex(graphTrans.getRotate_x_num());
	y_theta_combobox_->setCurrentIndex(graphTrans.getRotate_y_num());
	z_theta_combobox_->setCurrentIndex(graphTrans.getRotate_z_num());

	if (transLineEidt)
	{
		Mirror* tmpmirror = dynamic_cast<Mirror*>(mirror);
		transLineEidt->setText(QString::number(tmpmirror->getTransEffi()));
	}

	connect(ULineEidt, SIGNAL(textChanged(QString)),
		this, SLOT(on_GraphChange(QString)));
	connect(VLineEidt, SIGNAL(textChanged(QString)),
		this, SLOT(on_GraphChange(QString)));
	connect(NLineEidt, SIGNAL(textChanged(QString)),
		this, SLOT(on_GraphChange(QString)));

	connect(xthetaLineEidt, SIGNAL(textChanged(QString)),
		this, SLOT(on_GraphChange(QString)));
	connect(ythetaLineEidt, SIGNAL(textChanged(QString)),
		this, SLOT(on_GraphChange(QString)));
	connect(zthetaLineEidt, SIGNAL(textChanged(QString)),
		this, SLOT(on_GraphChange(QString)));

	connect(x_theta_combobox_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_ComboboxChange(int)));
	connect(y_theta_combobox_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_ComboboxChange(int)));
	connect(z_theta_combobox_, SIGNAL(currentIndexChanged(int)),
		this, SLOT(on_ComboboxChange(int)));
}

void GraphTransWidget::closeEvent(QCloseEvent * event)
{
	emit sendData(0);
}


void GraphTransWidget::SaveParam(Json::Value* param_js_ptr) {
	if (param_js_ptr == nullptr) return;
	Json::Value& param_js = *param_js_ptr;
	SAVE_EDIT_TO_JSON(ULineEidt, param_js);
	SAVE_EDIT_TO_JSON(VLineEidt, param_js);
	SAVE_EDIT_TO_JSON(NLineEidt, param_js);
	SAVE_EDIT_TO_JSON(xthetaLineEidt, param_js);
	SAVE_EDIT_TO_JSON(ythetaLineEidt, param_js);
	SAVE_EDIT_TO_JSON(zthetaLineEidt, param_js);
}

bool GraphTransWidget::LoadParam(const Json::Value& param_js) {
	try
	{
		param_js.isMember("xx");
		READ_EDIT_TO_JSON(ULineEidt, param_js);
		READ_EDIT_TO_JSON(VLineEidt, param_js);
		READ_EDIT_TO_JSON(NLineEidt, param_js);
		READ_EDIT_TO_JSON(xthetaLineEidt, param_js);
		READ_EDIT_TO_JSON(ythetaLineEidt, param_js);
		READ_EDIT_TO_JSON(zthetaLineEidt, param_js);

	}
	catch (const std::exception&)
	{
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("读取 GraphTransWidget 失败"));
		return false;
	}
	return true;
}