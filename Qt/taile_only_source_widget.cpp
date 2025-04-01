#include "taile_only_source_widget.h"
#include <QFileDialog>
#include "../def.h"
#include <QDir>
#include <QMessageBox>
#include <QCoreApplication>
#include <QProcess>
#include <QProgressDialog>
#include <vtkjsoncpp/json/json.h>
#include <QThread>

#include "global_config.h"

TaileOnlySourceWidget::TaileOnlySourceWidget(QWidget *parent)
{
	setWindowTitle(QString::fromLocal8Bit("创建理想泰勒源"));
	// page1

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
	tabLayout1->addWidget(baseGroupBox);
	tabLayout1->addWidget(dimGroupBox);
	tabLayout1->addWidget(rotateGroupBox);

	QWidget * widget1 = new QWidget;
	widget1->setLayout(tabLayout1);


	// page3
	InitSourceParam();

	//tabLayout2
	QVBoxLayout * tabLayout2; // page1
	tabLayout2 = new QVBoxLayout;
	tabLayout2->addWidget(basic_qbox_);

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

void TaileOnlySourceWidget::InitSourceParam() {
	theta_label_ = new QLabel(QString::fromLocal8Bit("俯仰角(°):"));
	phi_label_ = new QLabel(QString::fromLocal8Bit("方位角(°):"));
	fre_label_ = new QLabel(QString::fromLocal8Bit("频率(GHz):"));
	radius_label_ = new QLabel(QString::fromLocal8Bit("口径场半径:"));
	s11_label_ = new QLabel(QString::fromLocal8Bit("旁瓣电平:"));
	polarization_type_lable_ = new QLabel(QString::fromLocal8Bit("极化方式:"));
	dx_label_ = new QLabel(QString::fromLocal8Bit("x方向上阵元间距(波长):"));
	dy_label_ = new QLabel(QString::fromLocal8Bit("y方向上阵元间距(波长):"));
	ds_label_ = new QLabel(QString::fromLocal8Bit("ds:"));

	theta_edit_ = new QLineEdit("0");
	phi_edit_ = new QLineEdit("0");
	fre_edit_ = new QLineEdit("17");
	radius_edit_ = new QLineEdit("0.075");
	s11_edit_ = new QLineEdit("30");
	dx_edit_ = new QLineEdit("0.5");
	dy_edit_ = new QLineEdit("0.5");
	ds_edit_ = new QLineEdit("1e-3");
	polarization_type_combobox_ = new QComboBox;
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("水平极化Ex"));
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("垂直极化Ey"));

	source_diff_lable_ = new QLabel(QString::fromLocal8Bit("波束类型:"));
	source_diff_combobox_ = new QComboBox;
	source_diff_combobox_->addItem(QString::fromLocal8Bit("和波束"));
	source_diff_combobox_->addItem(QString::fromLocal8Bit("差波束"));

	QGridLayout* basic_layout = new QGridLayout;
	basic_layout->addWidget(theta_label_, 0, 0);
	basic_layout->addWidget(theta_edit_, 0, 1);
	basic_layout->addWidget(phi_label_, 1, 0);
	basic_layout->addWidget(phi_edit_, 1, 1);
	basic_layout->addWidget(fre_label_, 2, 0);
	basic_layout->addWidget(fre_edit_, 2, 1);
	basic_layout->addWidget(radius_label_, 3, 0);
	basic_layout->addWidget(radius_edit_, 3, 1);
	basic_layout->addWidget(s11_label_, 4, 0);
	basic_layout->addWidget(s11_edit_, 4, 1);
	basic_layout->addWidget(ds_label_, 8, 0);
	basic_layout->addWidget(ds_edit_, 8, 1);
	basic_layout->addWidget(polarization_type_lable_, 9, 0);
	basic_layout->addWidget(polarization_type_combobox_, 9, 1);
	basic_layout->addWidget(source_diff_lable_, 10, 0);
	basic_layout->addWidget(source_diff_combobox_, 10, 1);
	basic_qbox_ = new QGroupBox(QString::fromLocal8Bit("参数配置"));
	basic_qbox_->setLayout(basic_layout);
}

TaileOnlySourceWidget::~TaileOnlySourceWidget()
{
	
}

bool TaileOnlySourceWidget::getField(Field *& ptr)
{
	if (!GetGraphTransOk()) {
		return false;
	}
	if (!GenSource()) {
		QMessageBox::warning(NULL, "Warning", QString::fromLocal8Bit("生成源失败"));
		return false;
	}
	bool ok, ok_back;
	vector<double> para(6);
	string temp;
	para[0] = planeMirror->getWidth();
	para[1] = planeMirror->getDepth();	
	/*para[2] = z0LineEdit->text().toDouble(&ok);
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
	ptr = new Gaussain(planeMirror->getGraphTrans(), para);*/

	ptr = new Field();
	ptr->setGraphTrans(planeMirror->getGraphTrans());
	ptr->SetFre(fre_ * 1e9);
	ptr->ReadPythonText(dir_path_ + "/source.txt");



	QTreeWidgetItem* tree = new QTreeWidgetItem;
	tree->setText(0, QString::fromLocal8Bit("理想泰勒源"));

	QTreeWidgetItem* treeWidth = new QTreeWidgetItem;
	treeWidth->setText(0, QString::fromLocal8Bit("俯仰角(°):") + QString::number(theta_));
	tree->addChild(treeWidth);

	treeWidth = new QTreeWidgetItem;
	treeWidth->setText(0, QString::fromLocal8Bit("方位角(°):") + QString::number(phi_));
	tree->addChild(treeWidth);

	treeWidth = new QTreeWidgetItem;
	treeWidth->setText(0, QString::fromLocal8Bit("频率(GHz):") + QString::number(fre_));
	tree->addChild(treeWidth);

	treeWidth = new QTreeWidgetItem;
	treeWidth->setText(0, QString::fromLocal8Bit("口径场半径:") + QString::number(radius_));
	tree->addChild(treeWidth);

	treeWidth = new QTreeWidgetItem;
	treeWidth->setText(0, QString::fromLocal8Bit("极化方式:") + polarization_type_ == 1 ? QString::fromLocal8Bit("垂直极化Ey") : QString::fromLocal8Bit("水平极化Ey"));
	tree->addChild(treeWidth);


	treeWidth = new QTreeWidgetItem;
	treeWidth->setText(0, QString::fromLocal8Bit("ds:") + QString::number(ds_));
	tree->addChild(treeWidth);

	ptr->SetQTreeWidgetItem(tree);
	return true;
}

void TaileOnlySourceWidget::setMirror(Mirror * ptr)
{
	planeMirror = dynamic_cast<PlaneMirror*>(ptr); ;
	GraphTransWidget::setMirror(ptr);

	connect(widthLineEdit, SIGNAL(textChanged(QString)),
		this, SLOT(on_widthChange(QString)));
	connect(depthLineEdit, SIGNAL(textChanged(QString)),
		this, SLOT(on_depthChange(QString)));
}

void TaileOnlySourceWidget::on_widthChange(QString var)
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

void TaileOnlySourceWidget::on_depthChange(QString var)
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

bool TaileOnlySourceWidget::ReadScanParam() {
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(theta_edit_, theta_, -0.1, 360.1);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(phi_edit_, phi_, -0.1, 360.1);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(fre_edit_, fre_, 0, 1e8);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(radius_edit_, radius_, 0, 1000);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(s11_edit_, s11_, 0, 1000);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(ds_edit_, ds_, 0.0, 100);


	mesh_N_ = planeMirror->getWidth() / ds_ + 1;

	polarization_type_ = polarization_type_combobox_->currentIndex();
	source_diff_flag_ = source_diff_combobox_->currentIndex();

	return true;
}

void TaileOnlySourceWidget::GenSourceMeta() {

	Json::Value js;
	js["version"] = 1;
	js["step"] = 0;
	std::string path = std::string(QCoreApplication::applicationDirPath().toStdString());

	js["result_path"] = dir_path_;
	js["theta"] = theta_;
	js["phi"] = phi_;
	js["fre"] = fre_ * 1e9;
	js["radius"] = radius_;
	js["s11"] = s11_;
	js["ds"] = ds_;
	js["mesh_n"] = mesh_N_;

	std::ofstream outfile(dir_path_ + "/source_meta.json");
	if (!outfile.is_open())
	{
		return;
	}
	outfile << js.toStyledString();
	outfile.close();
}

bool TaileOnlySourceWidget::IsPythonWorkSuccess(const std::string& status_file) {
	std::ifstream ifs;
	ifs.open(status_file, std::ios::in);
	if (ifs.is_open()) {
		std::string temp;
		getline(ifs, temp);
		if (!temp.empty() && temp[0] == '1') {
			return true;
		}
		ifs.close();
	}
	return false;
}

bool TaileOnlySourceWidget::GenSource() {
	if (!ReadScanParam()) {
		QMessageBox::warning(NULL, "Warning", QString::fromLocal8Bit("填写参数有误"));
		return false;
	}
	dir_path_ = QCoreApplication::applicationDirPath().toStdString() + "/output/real_taile_source";
	QDir dir;
	dir.mkpath(dir_path_.c_str());
	GenSourceMeta();
	QProcess process;

	if (GlobalConfig::Instance()->IsQuickCalcByPython()) {
		process.start("python ",
			QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/sourceGenerateTaylor.py"
			<< std::string(dir_path_ + "/source_meta.json").c_str());
	}
	else {
		std::string exe_file = std::string(QCoreApplication::applicationDirPath().toStdString()
			+ "/script/quick_calc/sourceGenerateTaylor.exe ");
		process.start(exe_file.c_str(),
			QStringList() << std::string(dir_path_ + "/source_meta.json").c_str());
	}
	QProgressDialog progressDialog("Loading...", "Cancel", 0, 300, nullptr);
	progressDialog.setWindowModality(Qt::ApplicationModal);
	progressDialog.setWindowTitle("Loading");
	progressDialog.show();
	this->setHidden(true);
	// 等待进程结束，并显示进度条
	int i = 0;
	while (process.state() != QProcess::NotRunning) {
		progressDialog.setValue(i++);
		i = i % 300;
		QThread::msleep(100);
		QCoreApplication::processEvents();
	}
	progressDialog.close();
	std::string status_file = dir_path_ + "/source_status.txt";

	if (!IsPythonWorkSuccess(status_file)) {
		return false;
	}
	return true;
}

