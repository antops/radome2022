#include "scan_param_widget.h"
#include <QDir>

#include "../def.h"
#include "quick_calc_thread.h"
#include "global_config.h"
#include "init_scan_env_thread.h"
#include "../tools/calc_tbl.h"


bool IsPythonWorkSuccess(const std::string& status_file) {
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

ScanParamWidget::ScanParamWidget(DataManager* data_manager, int scan_widget_index, QWidget *parent)
	: data_manager_(data_manager), scan_widget_index_(scan_widget_index), QDialog(parent)
{
	theta_label_ = new QLabel(QString::fromLocal8Bit("������(��):"));
	phi_label_ = new QLabel(QString::fromLocal8Bit("��λ��(��):"));
	fre_label_ = new QLabel(QString::fromLocal8Bit("Ƶ��(GHz):"));
	radius_label_ = new QLabel(QString::fromLocal8Bit("�ھ����뾶:"));
	s11_label_ = new QLabel(QString::fromLocal8Bit("�԰��ƽ:"));
	polarization_type_lable_ = new QLabel(QString::fromLocal8Bit("������ʽ:"));
	dx_label_ = new QLabel(QString::fromLocal8Bit("x��������Ԫ���(����):"));
	dy_label_ = new QLabel(QString::fromLocal8Bit("y��������Ԫ���(����):"));
	mesh_N_label_ = new QLabel(QString::fromLocal8Bit("������"));
	ds_label_ = new QLabel(QString::fromLocal8Bit("ds:"));

	theta_edit_ = new QLineEdit("0");
	phi_edit_ = new QLineEdit("0");
	fre_edit_ = new QLineEdit("17");
	radius_edit_ = new QLineEdit("0.075");
	s11_edit_ = new QLineEdit("30");
	dx_edit_ = new QLineEdit("0.5");
	dy_edit_ = new QLineEdit("0.5");
	mesh_N_edit_ = new QLineEdit("161");
	ds_edit_ = new QLineEdit("1e-3");
	polarization_type_combobox_ = new QComboBox;
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("ˮƽ����Ex"));
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("��ֱ����Ey"));
	

	path_lable_ = new QLabel(QString::fromLocal8Bit("�������ļ�����:"));
	path_edit_ = new QLineEdit;
	browse_btn_ = new QPushButton(tr("Browse..."));
	connect(browse_btn_, SIGNAL(clicked()), this, SLOT(OnBrowseBtn()));

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
	basic_layout->addWidget(dx_label_, 5, 0);
	basic_layout->addWidget(dx_edit_, 5, 1);
	basic_layout->addWidget(dy_label_, 6, 0);
	basic_layout->addWidget(dy_edit_, 6, 1);
	basic_layout->addWidget(mesh_N_label_, 7, 0);
	basic_layout->addWidget(mesh_N_edit_, 7, 1);
	basic_layout->addWidget(ds_label_, 8, 0);
	basic_layout->addWidget(ds_edit_, 8, 1);
	basic_layout->addWidget(polarization_type_lable_, 9, 0);
	basic_layout->addWidget(polarization_type_combobox_, 9, 1);
	basic_layout->addWidget(path_lable_, 10, 0);
	QHBoxLayout* layout_tmp = new QHBoxLayout;
	layout_tmp->addWidget(path_edit_);
	layout_tmp->addWidget(browse_btn_);
	basic_layout->addLayout(layout_tmp, 10, 1);
	basic_qbox_ = new QGroupBox(QString::fromLocal8Bit("Դ��Ϣ����"));
	basic_qbox_->setLayout(basic_layout);

	// Զ����Ϣ
	theta_begin_lable_ = new QLabel(QString::fromLocal8Bit("theta��ʼ��:"));
	theta_begin_edit_ = new QLineEdit("0");
	phi_begin_lable_ = new QLabel(QString::fromLocal8Bit("phi��ʼ��:"));
	phi_begin_edit_ = new QLineEdit("0");

	theta_end_lable_ = new QLabel(QString::fromLocal8Bit("theta������:"));
	theta_end_edit_ = new QLineEdit("180");
	phi_end_lable_ = new QLabel(QString::fromLocal8Bit("phi������:"));
	phi_end_edit_ = new QLineEdit("360");

	theta_gap_lable_ = new QLabel(QString::fromLocal8Bit("theta����:"));
	theta_gap_edit_ = new QLineEdit("1.0");
	phi_gap_lable_ = new QLabel(QString::fromLocal8Bit("phi����:"));
	phi_gap_edit_ = new QLineEdit("1.0");

	QGridLayout* farfield_layout = new QGridLayout;
	farfield_layout->addWidget(theta_begin_lable_, 0, 0);
	farfield_layout->addWidget(theta_begin_edit_, 0, 1);
	farfield_layout->addWidget(phi_begin_lable_, 1, 0);
	farfield_layout->addWidget(phi_begin_edit_, 1, 1);
	farfield_layout->addWidget(theta_end_lable_, 2, 0);
	farfield_layout->addWidget(theta_end_edit_, 2, 1);
	farfield_layout->addWidget(phi_end_lable_, 3, 0);
	farfield_layout->addWidget(phi_end_edit_, 3, 1);
	farfield_layout->addWidget(theta_gap_lable_, 4, 0);
	farfield_layout->addWidget(theta_gap_edit_, 4, 1);
	farfield_layout->addWidget(phi_gap_lable_, 5, 0);
	farfield_layout->addWidget(phi_gap_edit_, 5, 1);
	farfield_box_ = new QGroupBox(QString::fromLocal8Bit("Զ����Ϣ����"));
	farfield_box_->setLayout(farfield_layout);

	// scan_qbox_
	scan_param_label_ = new QLabel(QString::fromLocal8Bit("ɨ�����ѡ��:"));
	scan_param_combobox_ = new QComboBox;
	scan_param_combobox_->addItem(QString::fromLocal8Bit("������(��)"));
	scan_param_combobox_->addItem(QString::fromLocal8Bit("��λ��(��)"));
	scan_param_combobox_->addItem(QString::fromLocal8Bit("Ƶ��(GHz)"));
	min_label_ = new QLabel(QString::fromLocal8Bit("��Сֵ:"));
	max_label_ = new QLabel(QString::fromLocal8Bit("���ֵ:"));
	step_label_ = new QLabel(QString::fromLocal8Bit("���:"));
	min_edit_ = new QLineEdit("0");
	max_edit_ = new QLineEdit("90");
	step_edit_ = new QLineEdit("15");

	ok_qbt_ = new QPushButton(QString::fromLocal8Bit("ȷ��"));
	connect(ok_qbt_, SIGNAL(clicked()), this, SLOT(OnOKClicked()));


	QGridLayout* scan_layout = new QGridLayout;
	scan_layout->addWidget(scan_param_label_, 0, 0);
	scan_layout->addWidget(scan_param_combobox_, 0, 1);
	scan_layout->addWidget(min_label_, 1, 0);
	scan_layout->addWidget(min_edit_, 1, 1);
	scan_layout->addWidget(max_label_, 2, 0);
	scan_layout->addWidget(max_edit_, 2, 1);
	scan_layout->addWidget(step_label_, 3, 0);
	scan_layout->addWidget(step_edit_, 3, 1);

	scan_qbox_ = new QGroupBox(QString::fromLocal8Bit("ɨ��������"));
	scan_qbox_->setLayout(scan_layout);

	QVBoxLayout* v_layout = new QVBoxLayout();
	v_layout->addWidget(farfield_box_);
	v_layout->addWidget(scan_qbox_);


	QHBoxLayout *param_layout = new QHBoxLayout();
	param_layout->addWidget(basic_qbox_);
	param_layout->addLayout(v_layout);
	param_layout->addWidget(ok_qbt_);

	result_table_widget_ = new QTableWidget(10, 4, this);
	result_table_widget_->setFont(QFont("����", 15));
	//���ñ����ÿһ�еı�ͷ
	result_table_widget_->setHorizontalHeaderLabels(QStringList() 
		<< QString::fromLocal8Bit("������(��)")
		<< QString::fromLocal8Bit("��λ��(��)")
		<< QString::fromLocal8Bit("Ƶ��(GHz)")
		<< QString::fromLocal8Bit("͸����"));
	//���ñ���������ڵ����е�Ԫ�񶼲�����༭
	result_table_widget_->setEditTriggers(QAbstractItemView::NoEditTriggers);


	QVBoxLayout* result_layout = new QVBoxLayout;
	result_layout->addWidget(result_table_widget_);

	result_qbox_ = new QGroupBox(QString::fromLocal8Bit("�������"));
	result_qbox_->setLayout(result_layout);

	result_pic_plot_ = new QCustomPlot;
	result_pic_plot_->xAxis->setLabel("x");
	result_pic_plot_->yAxis->setLabel(QString::fromLocal8Bit("͸����"));
	result_pic_plot_->setMinimumHeight(250);

	QVBoxLayout* result_pic_layout = new QVBoxLayout;
	result_pic_layout->addWidget(result_pic_plot_);

	result_pic_qbox_ = new QGroupBox(QString::fromLocal8Bit("�������ͼ"));
	result_pic_qbox_->setLayout(result_pic_layout);


	QVBoxLayout* main_layout = new QVBoxLayout(this);
	main_layout->addLayout(param_layout);
	main_layout->addWidget(result_qbox_);
	main_layout->addWidget(result_pic_qbox_);

}

ScanParamWidget::~ScanParamWidget()
{
}

void ScanParamWidget::OnOKClicked() {
	if (!ReadScanParam()) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("��ȡ��������"));
		return;
	}
	if (!Islegal()) {
		return;
	}

	if (!InitEnv()) {
		return;
	}
	QDir dir;

	for (int i = 0; i <= scan_count_; i++) {
		cur_value_ = i * step_ + min_;
		if (i * step_ + min_ > max_) {
			cur_value_ = max_;
		}
		if (scan_type_ == 0) {
			theta_ = cur_value_;
		}
		else if (scan_type_ == 1) {
			phi_ = cur_value_;
		}
		else {
			fre_ = cur_value_;
		}
		cur_index_ = i;
		cur_result_path_ = result_path_ + std::to_string(i);
		dir.mkpath(cur_result_path_.c_str());

		process_show_widget_->QAppend(" ");
		process_show_widget_->QAppend(QString::fromLocal8Bit(">>>>>>>>>>>��ʼ�����") 
			+ QString::number(i+1) + QString::fromLocal8Bit("������<<<<<<<<<<"));

		process_show_widget_->SetWidgetText(QString::fromLocal8Bit("���ڽ��п��ټ��� (")
			+QString::number(i + 1) + QString::fromLocal8Bit("/") + QString::number(scan_count_ + 1)
			+ QString::fromLocal8Bit(") ����ȴ��������......"));

		if (!GenSource()) {
			return;
		}

		
		if (!QuickCalc()) {
			return;
		}


		process_show_widget_->QAppend(QString::fromLocal8Bit(">>>>>>>>>>>�����")
			+ QString::number(i + 1) + QString::fromLocal8Bit("���������<<<<<<<<<<"));
	}

	process_show_widget_->close();
}

bool ScanParamWidget::InitEnv() {
	process_show_widget_ = new ProcessShowWidget;
	process_show_widget_->SetWidgetText(QString::fromLocal8Bit("���ڽ��п��ټ��㣬��ȴ��������..."));
	process_show_widget_->setWindowFlags(Qt::WindowStaysOnTopHint);
	process_show_widget_->resize(800, 600);
	// process_show_widget_->SetTextBrowserHidden(true);
	process_show_widget_->show();

	process_show_widget_->QAppend(QString::fromLocal8Bit("��ʼɨ�Σ���ȴ����..."));

	InitScanEnvThread thread(*data_manager_, process_show_widget_);
	thread.start();
	while (!thread.isFinished()) {
		// this->processEvents();
		QCoreApplication::processEvents();
	}
	if (thread.GetStatus() == -200) {
		process_show_widget_->close();
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("δ����ģ��!"));

		return false;
	}

	QDateTime data_time = QDateTime::currentDateTime();
	QString current_time = data_time.toString("yyyy_MM_dd_hh_mm_ss");
	dir_path_ = QCoreApplication::applicationDirPath().toStdString() + "/output";
	if (!custom_path_.empty()) {
		dir_path_ = custom_path_;
	}
	result_path_ = dir_path_ + "/result/" + current_time.toStdString() + "_scan/";
	QDir dir;
	dir.mkpath(result_path_.c_str());
	stl_path_vec_ = thread.GetStlPathVec();

	process_show_widget_->QAppend(QString::fromLocal8Bit("������������:") + QString::fromLocal8Bit(result_path_.c_str()));
	return true;
}

bool ScanParamWidget::ReadScanParam() {
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(theta_edit_, theta_, -0.1, 360.1);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(phi_edit_, phi_, -0.1, 360.1);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(fre_edit_, fre_, 0, 1e8);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(radius_edit_, radius_, 0, 1000);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(s11_edit_, s11_, 0, 1000);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(min_edit_, min_, 0, 1e16);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(max_edit_, max_, 0, 1e16);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(step_edit_, step_, 0, 1e16);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(dx_edit_, dx_, 0.0, 100);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(dy_edit_, dy_, 0.0, 100);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(ds_edit_, ds_, 0.0, 100);
	PARSE_EDIT_LINE_TO_INT_RETURN(mesh_N_edit_, mesh_N_, 1, 10000);

	polarization_type_ = polarization_type_combobox_->currentIndex();
	scan_type_ = scan_param_combobox_->currentIndex();
	if (!ReadTheta()) {
		return false;
	}
	if (!ReadPhi()) {
		return false;
	}
	
	return true;
}

bool ScanParamWidget::Islegal() {
	if (step_ < 0.000001) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("������������"));
		return false;
	}
	scan_count_ = (max_ - min_) / step_;
	if (scan_count_ < 0 || scan_count_ > 1000) {
		QMessageBox::warning(NULL, "Warning",
			QString::fromLocal8Bit("�������ɨ�θ���"));
		return false;
	}

	result_table_widget_->setRowCount(scan_count_+1);
	if (scan_type_ == 0) {
		result_pic_plot_->xAxis->setLabel(QString::fromLocal8Bit("������(��)"));
	}
	else if (scan_type_ == 1) {
		result_pic_plot_->xAxis->setLabel(QString::fromLocal8Bit("��λ��(��)"));

	}
	else {
		result_pic_plot_->xAxis->setLabel(QString::fromLocal8Bit("Ƶ��(GHz)"));

	}
	return true;
}

void ScanParamWidget::GenSourceMeta() {
	Json::Value js;
	js["version"] = 1;
	js["step"] = cur_index_;
	std::string path = std::string(QCoreApplication::applicationDirPath().toStdString());

	js["result_path"] = cur_result_path_;
	js["theta"] = theta_;
	js["phi"] = phi_;
	js["fre"] = fre_*1e9;
	js["radius"] = radius_;
	js["s11"] = s11_;
	js["dx"] = dx_;
	js["dy"] = dy_;
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

bool ScanParamWidget::GenSource() {
	process_show_widget_->QAppend(QString::fromLocal8Bit("����Դ�ļ�"));
	GenSourceMeta();
	QProcess process;

	if (GlobalConfig::Instance()->IsQuickCalcByPython()) {
		process_show_widget_->QAppend(
			QCoreApplication::applicationDirPath() + "/script/quick_calc_py/sourceGenerate.py");
		process_show_widget_->QAppend(
			std::string(dir_path_ + "/source_meta.json").c_str());
		process.start("python ",
			QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/sourceGenerate.py"
			<< std::string(dir_path_ + "/source_meta.json").c_str());
	}
	else {
		process_show_widget_->QAppend(
			QCoreApplication::applicationDirPath() + "/script/quick_calc_py/sourceGenerate.exe");
		process_show_widget_->QAppend(
			std::string(dir_path_ + "/source_meta.json").c_str());
		std::string exe_file = std::string(QCoreApplication::applicationDirPath().toStdString()
			+ "/script/quick_calc/sourceGenerate.exe ");
		process.start(exe_file.c_str(),
			QStringList() << std::string(dir_path_ + "/source_meta.json").c_str());
	}

	process_show_widget_->SetProcess(&process);
	connect(&process, SIGNAL(readyReadStandardOutput()), process_show_widget_, SLOT(OnReadoutput()));
	connect(&process, SIGNAL(readyReadStandardError()), process_show_widget_, SLOT(OnReadError()));


	// �ȴ����̽���������ʾ������
	while (process.state() != QProcess::NotRunning) {
		if (process_show_widget_->IsCanceled()) {
			process.kill();
			break;
		}
		QCoreApplication::processEvents();
	}
	std::string status_file = cur_result_path_ + "/source_status.txt";

	if (!IsPythonWorkSuccess(status_file)) {
		process_show_widget_->QAppend(QString::fromLocal8Bit("����Դ�ļ�ʧ�ܣ�����"));
		return false;
	}

	process_show_widget_->QAppend(QString::fromLocal8Bit("����Դ�ļ����"));
}

void ScanParamWidget::GenCalcMeta() {
	GenQuickCalcMeta gen_meta(*data_manager_);
	QDir dir;
	cur_result_with_radome_path_ = cur_result_path_ + "/r/";
	dir.mkpath(cur_result_with_radome_path_.c_str());
	gen_meta.SetResultPath(cur_result_with_radome_path_);

	gen_meta.SetDs(ds_);

	cur_result_non_radome_path_ = cur_result_path_ + "/nr/";
	dir.mkpath(cur_result_non_radome_path_.c_str());
	gen_meta.SetResultNonRadomePath(cur_result_non_radome_path_);

	process_show_widget_->QAppend(QString::fromLocal8Bit("path:")
		+ QString::fromLocal8Bit(dir_path_.c_str()));

	// ��������
	std::string source_path = cur_result_path_ + "/source.txt";
	//// ������
	//auto source = data_manager_->GetSource();
	//source->SaveFieldBase(source_path);
	gen_meta.SetSourcePath(source_path);

	// ����ģ��	
	for (auto& x : stl_path_vec_) {
		gen_meta.AddSTLPath(x);
	}
	gen_meta.SetOutputPath(cur_result_path_);
	gen_meta.SetFre(fre_ * 1e9);
	gen_meta.SetPolarizationType(polarization_type_);
	gen_meta.SetCalcConf(GetCalcConf());

	gen_meta.WriteMetaMsg(cur_result_path_ + "/meta.json");

}

bool ScanParamWidget::QuickCalc() {
	// ����QProcess���󣬲�����Python����·���Ͳ���
	QProcess process;
	process_show_widget_->QAppend(QString::fromLocal8Bit(" "));
	process_show_widget_->QAppend(QString::fromLocal8Bit("��ʼ���㣬��ȴ����..."));

	GenCalcMeta();

	if (GlobalConfig::Instance()->IsQuickCalcByPython()) {
		process_show_widget_->QAppend(
			QCoreApplication::applicationDirPath() + "/script/quick_calc_py/mainforNlayer.py");
		process_show_widget_->QAppend(
			std::string(cur_result_path_ + "/meta.json").c_str());
		process.start("python ",
			QStringList() << QCoreApplication::applicationDirPath() + "/script/quick_calc_py/mainforNlayer.py"
			<< std::string(cur_result_path_ + "/meta.json").c_str());
	}
	else {
		std::string exe_file = std::string(QCoreApplication::applicationDirPath().toStdString()
			+ "/script/quick_calc/mainforNlayer.exe ");
		process_show_widget_->QAppend(exe_file.c_str());
		process_show_widget_->QAppend(
			std::string(cur_result_path_ + "/meta.json").c_str());
		process.start(exe_file.c_str(),
			QStringList() << std::string(cur_result_path_ + "/meta.json").c_str());
	}

	process_show_widget_->SetProcess(&process);
	connect(&process, SIGNAL(readyReadStandardOutput()), process_show_widget_, SLOT(OnReadoutput()));
	connect(&process, SIGNAL(readyReadStandardError()), process_show_widget_, SLOT(OnReadError()));


	// �ȴ����̽���������ʾ������
	while (process.state() != QProcess::NotRunning) {
		if (process_show_widget_->IsCanceled()) {
			process.kill();
			break;
		}
		QCoreApplication::processEvents();
	}

	std::string status_file = cur_result_with_radome_path_ + "/status.txt";
	if (!IsPythonWorkSuccess(status_file)) {
		process_show_widget_->QAppend(QString::fromLocal8Bit("����ʧ�ܣ�����"));
		return false;
	}

	if (!ReadResult()) {
		process_show_widget_->QAppend(QString::fromLocal8Bit("��ȡ����ʧ�ܣ�����"));
		return false;
	}

	if (!GenTbl()) {
		process_show_widget_->QAppend(QString::fromLocal8Bit("����͸����ʧ�ܣ�����"));
		return false;
	}

	UpdateTable();

	process_show_widget_->QAppend(QString::fromLocal8Bit("��ʼ�������"));
	return true;
}

bool ScanParamWidget::ReadResult() {
	radome_far_field_ = new FarField("farfield");

	process_show_widget_->QAppend(QString::fromLocal8Bit(std::string(cur_result_with_radome_path_).c_str()));
	qDebug((std::string("read npy: ") + cur_result_with_radome_path_).c_str());
	if (!radome_far_field_->ReadFromNpy(cur_result_with_radome_path_)) {
		qDebug((std::string("read npy failed: ") + cur_result_with_radome_path_).c_str());;
		return false;
	}

	int index = data_manager_->SetFarSource(radome_far_field_);
	radome_far_field_->SetIndex(index);
	radome_far_field_->SetName(std::string("��") + std::to_string(scan_widget_index_) + std::string("��ɨ�ν��_") + std::to_string(cur_index_) + "_����" );
	radome_far_field_->SetPicFilePath(cur_result_with_radome_path_ + "/" + GlobalConfig::Instance()->GetFarfieldPicName());

	// field_tree_item_->addChild(far_field->GetTree());
	// field_tree_item_->setExpanded(true);

	process_show_widget_->QAppend(QString::fromLocal8Bit(std::string(cur_result_non_radome_path_).c_str()));
	non_radome_far_field_ = new FarField("non_radome_farfield");
	qDebug((std::string("read non npy: ") + cur_result_non_radome_path_).c_str());
	if (!non_radome_far_field_->ReadFromNpy(cur_result_non_radome_path_)) {
		qDebug((std::string("read npy failed: ") + cur_result_non_radome_path_).c_str());
		return false;
	}
	int non_index = data_manager_->SetFarSource(non_radome_far_field_);
	non_radome_far_field_->SetIndex(non_index);
	non_radome_far_field_->SetName(std::string("��")  + std::to_string(scan_widget_index_)
		+ std::string("��ɨ�ν��_") + std::to_string(cur_index_) + "_����");
	non_radome_far_field_->SetPicFilePath(cur_result_non_radome_path_
		+ "/" + GlobalConfig::Instance()->GetFarfieldPicName());
	return true;
}

bool ScanParamWidget::GenTbl() {
	auto calc_helper = CalcTbl(non_radome_far_field_, radome_far_field_);
	if (!calc_helper.Run()) {
		return false;
	}
	return true;
}

void ScanParamWidget::UpdateTable() {
	auto tbl_v = radome_far_field_->GetTBL();
	result_table_widget_->setItem(cur_index_, 0, new QTableWidgetItem(QString::number(theta_)));
	result_table_widget_->setItem(cur_index_, 1, new QTableWidgetItem(QString::number(phi_)));
	result_table_widget_->setItem(cur_index_, 2, new QTableWidgetItem(QString::number(fre_)));
	result_table_widget_->setItem(cur_index_, 3, new QTableWidgetItem(QString::number(tbl_v)));

	result_pic_x_.push_back(cur_value_);
	result_pic_y_.push_back(tbl_v);

	if (min_y_ > tbl_v) {
		min_y_ = tbl_v;
	}
	if (max_y_ < tbl_v) {
		max_y_ = tbl_v;
	}

	result_pic_plot_->clearGraphs();
	result_pic_plot_->addGraph();
	result_pic_plot_->graph(0)->setPen(QPen(QColor(255, 110, 40)));
	result_pic_plot_->graph(0)->setData(result_pic_x_, result_pic_y_);
	result_pic_plot_->graph(0)->setName("A");
	result_pic_plot_->xAxis->setRange(min_, max_);
	result_pic_plot_->yAxis->setRange(min_y_, max_y_ * 1.1);
	result_pic_plot_->replot();
}


bool ScanParamWidget::ReadTheta() {
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(theta_begin_edit_, min_theta_, -180.0000001, 180.0000001);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(theta_end_edit_, max_theta_, -180.0000001, 180.0000001);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(theta_gap_edit_, gap_theta_, -0.000001, 180.0000001);
	if (max_theta_ < min_theta_) {
		theta_begin_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		theta_end_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		return false;
	}
	num_theta_ = (max_theta_ - min_theta_) / gap_theta_ + 1;
}

bool ScanParamWidget::ReadPhi() {
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(phi_begin_edit_, min_phi_, -360.0000001, 360.0000001);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(phi_end_edit_, max_phi_, -360.0000001, 360.0000001);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(phi_gap_edit_, gap_phi_, -0.000001, 360.0000001);
	if (max_phi_ < min_phi_) {
		phi_begin_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		phi_end_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		return false;
	}
	num_phi_ = (max_phi_ - min_phi_) / gap_phi_ + 1;
}

void ScanParamWidget::OnBrowseBtn() {

	//// ���ļ���ѡ��Ի���
	//QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
	//	QDir::homePath(),
	//	QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	//if (!folderPath.isEmpty()) {
	//	// ���ѡ����ļ���·��
	//	qDebug() << "Selected folder path: " << folderPath;
	//	// �����������㴦����ļ���·���Ĵ���
	//	custom_path_ = folderPath.toStdString();
	//	path_edit_->setText(folderPath);
	//}

	QString selectedDir = QFileDialog::getSaveFileName(nullptr, QString::fromLocal8Bit("�����ļ�"), QDir::homePath(), QString::fromLocal8Bit(""));
	if (!selectedDir.isEmpty()) {
		QDir dir(selectedDir);
		// ����ļ����Ƿ����
		if (!dir.exists()) {
			// ���������򴴽��ļ���
			if (!dir.mkpath(selectedDir)) {
				QMessageBox::critical(nullptr, QString::fromLocal8Bit("ʧ��"), QString::fromLocal8Bit("�ļ��д���ʧ�ܣ�"));
			}
		}
		qDebug() << "Selected folder path: " << selectedDir;
		// �����������㴦����ļ���·���Ĵ���
		custom_path_ = selectedDir.toStdString();
		path_edit_->setText(selectedDir);
	}
}