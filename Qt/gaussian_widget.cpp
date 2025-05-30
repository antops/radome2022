#include "gaussian_widget.h"
#include <QFileDialog>

GaussianWidget::GaussianWidget(QWidget *parent)
{
	setWindowTitle(QString::fromLocal8Bit("创建高斯源"));

	// page1
	//defGroupBox
	defGroupBox = new QGroupBox;
	addDefGroupBox(defGroupBox, "Qt/images/rectangle_coor.png");

	//baseGroupBox
	baseGroupBox = new QGroupBox;
	addBaseGroupBox(baseGroupBox);

	//dimGroupBox
	widthlabel = new QLabel(tr("Width(W)"));
	depthlabel = new QLabel(tr("Depth(D)"));

	widthLineEdit = new QLineEdit;
	depthLineEdit = new QLineEdit;

	widthLineEdit->setText(tr("1"));
	depthLineEdit->setText(tr("1"));

	QGridLayout * layout3= new QGridLayout;
	layout3->addWidget(widthlabel, 0, 0);
	layout3->addWidget(depthlabel, 1, 0);
	layout3->addWidget(widthLineEdit, 0, 1);
	layout3->addWidget(depthLineEdit, 1, 1);

	dimGroupBox = new QGroupBox;
	dimGroupBox->setTitle(tr("Dimensions"));
	dimGroupBox->setLayout(layout3);

	rotateGroupBox = new QGroupBox;
	addRotateGroupBox(rotateGroupBox);

	//tabLayout1
	QVBoxLayout * tabLayout1; // page1
	tabLayout1 = new QVBoxLayout;
	//tabLayout1->addWidget(defGroupBox);
	tabLayout1->addWidget(baseGroupBox);
	tabLayout1->addWidget(dimGroupBox);
	tabLayout1->addWidget(rotateGroupBox);

	QWidget * widget1 = new QWidget;
	widget1->setLayout(tabLayout1);

	// page2
	//QWidget * widget2 = new QWidget;
	//addRotateWidget(widget2, "Qt/images/RotateWidget.png");

	// page3
	imgGroupBox = new QGroupBox;
	addDefGroupBox(imgGroupBox, "Qt/images/Gaussian_def.png");

	//sourcegroup
	z0label = new QLabel(tr("Target distance(z0)"));
	w0label = new QLabel(tr("Gaussian waist(w0)"));
	frelabel = new QLabel(tr("Frequency  (GHz)"));
	dslabel = new QLabel(tr("Sampling Precision"));

	z0LineEdit = new QLineEdit;
	w0LineEdit = new QLineEdit;
	freLineEdit = new QLineEdit;
	DsLineEdit = new QLineEdit;

	z0LineEdit->setText("0");
	w0LineEdit->setText("0.04");
	freLineEdit->setText("10");
	DsLineEdit->setText("0.005");

	polarization_type_lable_ = new QLabel(QString::fromLocal8Bit("极化方向:"));
	polarization_type_combobox_ = new QComboBox;
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("水平极化Ex"));
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("垂直极化Ey"));

	source_diff_lable_ = new QLabel(QString::fromLocal8Bit("波束类型:"));
	source_diff_combobox_ = new QComboBox;
	source_diff_combobox_->addItem(QString::fromLocal8Bit("和波束"));
	source_diff_combobox_->addItem(QString::fromLocal8Bit("差波束"));
	connect(source_diff_combobox_, SIGNAL(currentIndexChanged(int)), this, SLOT(OnSourceDiffCombobox(int)));

	zero_dep_lable_ = new QLabel(QString::fromLocal8Bit("零深方向:"));
	zero_dep_combobox_ = new QComboBox;
	zero_dep_combobox_->addItem(QString::fromLocal8Bit("沿X轴"));
	zero_dep_combobox_->addItem(QString::fromLocal8Bit("沿Y轴"));
	zero_dep_lable_->setHidden(true);
	zero_dep_combobox_->setHidden(true);
	

	QGridLayout * layoutSou = new QGridLayout;
	layoutSou->addWidget(z0label, 0, 0);
	layoutSou->addWidget(w0label, 1, 0);
	layoutSou->addWidget(frelabel, 2, 0);
	layoutSou->addWidget(dslabel, 3, 0);
	layoutSou->addWidget(z0LineEdit, 0, 1);
	layoutSou->addWidget(w0LineEdit, 1, 1);
	layoutSou->addWidget(freLineEdit, 2, 1);
	layoutSou->addWidget(DsLineEdit, 3, 1);

	layoutSou->addWidget(polarization_type_lable_, 4, 0);
	layoutSou->addWidget(polarization_type_combobox_, 4, 1);

	layoutSou->addWidget(source_diff_lable_, 5, 0);
	layoutSou->addWidget(source_diff_combobox_, 5, 1);
	layoutSou->addWidget(zero_dep_lable_, 6, 0);
	layoutSou->addWidget(zero_dep_combobox_, 6, 1);

	sourceGroupBox = new QGroupBox;
	sourceGroupBox->setTitle(tr("Source"));
	sourceGroupBox->setLayout(layoutSou);

	//tabLayout2
	QVBoxLayout * tabLayout2; // page1
	tabLayout2 = new QVBoxLayout;
	tabLayout2->addWidget(imgGroupBox);
	tabLayout2->addWidget(sourceGroupBox);

	QWidget * widget3 = new QWidget;
	widget3->setLayout(tabLayout2);

	tabWidget = new QTabWidget;
	tabWidget->addTab(widget1, tr("Geometry"));
	tabWidget->addTab(widget3, tr("Excitation"));


	QGridLayout * layoutbtn = new QGridLayout;
	addBtn(layoutbtn);

	QVBoxLayout * mainlayout = new QVBoxLayout(this);
	mainlayout->addWidget(tabWidget);
	mainlayout->addLayout(layoutbtn);/**/

}

GaussianWidget::~GaussianWidget()
{
	
}

bool GaussianWidget::getField(Field *& ptr)
{
	if (!GetGraphTransOk()) {
		return false;
	}
	bool ok, ok_back;
	vector<double> para(9);
	string temp;
	para[0] = planeMirror->getWidth();
	para[1] = planeMirror->getDepth();	
	para[2] = z0LineEdit->text().toDouble(&ok);
	if (!ok)
	{		
		z0LineEdit->setStyleSheet("background-color:rgba(255,0,0,255)");
		return false;
	}
	para[3] = w0LineEdit->text().toDouble(&ok);
	if (!ok)
	{		
		w0LineEdit->setStyleSheet("background-color:rgba(255,0,0,255)");
		return false;
	}
	para[4] = freLineEdit->text().toDouble(&ok);
	if (!ok)
	{
		
		freLineEdit->setStyleSheet("background-color:rgba(255,0,0,255)");
		return false;
	}
	para[5] = DsLineEdit->text().toDouble(&ok);
	if (!ok)
	{		
		DsLineEdit->setStyleSheet("background-color:rgba(255,0,0,255)");
		return false;
	}
	para[6] = polarization_type_combobox_->currentIndex();

	para[7] = source_diff_combobox_->currentIndex();
	para[8] = zero_dep_combobox_->currentIndex();
	ptr = new Gaussain(planeMirror->getGraphTrans(), para);
	return true;
}

void GaussianWidget::setMirror(Mirror * ptr)
{
	planeMirror = dynamic_cast<PlaneMirror*>(ptr); ;
	GraphTransWidget::setMirror(ptr);

	connect(widthLineEdit, SIGNAL(textChanged(QString)),
		this, SLOT(on_widthChange(QString)));
	connect(depthLineEdit, SIGNAL(textChanged(QString)),
		this, SLOT(on_depthChange(QString)));
}

void GaussianWidget::on_widthChange(QString var)
{
	bool ok = false;
	double res = var.toDouble(&ok);
	if (!ok)
	{
		//输出参数有误
		widthLineEdit->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;

	}
	if (res <= 0.0 + THRESHOLD)
	{
		//输出参数有误 需要大于0
		widthLineEdit->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}
	planeMirror->setWidth(res);
	widthLineEdit->setStyleSheet("background-color:rgba(255,255,255,255)");
	emit sendData(2);
}

void GaussianWidget::on_depthChange(QString var)
{
	bool ok = false;
	double res = var.toDouble(&ok);
	if (!ok)
	{
		//输出参数有误
		depthLineEdit->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;
	}
	if (res <= 0.0 + THRESHOLD)
	{
		//输出参数有误 需要大于0
		depthLineEdit->setStyleSheet("background-color:rgba(255,0,0,255)");
		return;

	}
	planeMirror->setDepth(res);
	depthLineEdit->setStyleSheet("background-color:rgba(255,255,255,255)");
	emit sendData(2);
}


void GaussianWidget::OnSourceDiffCombobox(int index) {
	if (index == 1) {
		zero_dep_lable_->setHidden(false);
		zero_dep_combobox_->setHidden(false);
	}
	else {
		zero_dep_lable_->setHidden(true);
		zero_dep_combobox_->setHidden(true);
	}
}