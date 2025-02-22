#include "calc_config_widget.h"

#include <vtkjsoncpp/json/json.h>

#include "../def.h"

CalcConfigWidget::CalcConfigWidget(double fre_, QWidget *parent)
	: QDialog(parent)
{
	fre_lable_ = new QLabel(QString::fromLocal8Bit("频率(GHz):"));
	fre_edit_ = new QLineEdit(QString::number(fre_ / 1e9));

	polarization_type_lable_ = new QLabel(QString::fromLocal8Bit("极化方向:"));
	polarization_type_combobox_ = new QComboBox;
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("垂直极化Ey"));
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("水平极化Ex"));

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


	theta_begin_lable_ = new QLabel(QString::fromLocal8Bit("theta:"));
	theta_begin_edit_ = new QLineEdit("0");
	phi_begin_lable_ = new QLabel(QString::fromLocal8Bit("phi:"));
	phi_begin_edit_ = new QLineEdit("0");
	QGridLayout* begin_layout = new QGridLayout;
	begin_layout->addWidget(theta_begin_lable_, 0, 0);
	begin_layout->addWidget(theta_begin_edit_, 0, 1);
	begin_layout->addWidget(phi_begin_lable_, 1, 0);
	begin_layout->addWidget(phi_begin_edit_, 1, 1);
	begin_box_ = new QGroupBox(QString::fromLocal8Bit("起始点"));
	begin_box_->setLayout(begin_layout);
	

	theta_end_lable_ = new QLabel(QString::fromLocal8Bit("theta:"));
	theta_end_edit_ = new QLineEdit("180");
	phi_end_lable_ = new QLabel(QString::fromLocal8Bit("phi:"));
	phi_end_edit_ = new QLineEdit("360");
	QGridLayout* end_layout = new QGridLayout;
	end_layout->addWidget(theta_end_lable_, 0, 0);
	end_layout->addWidget(theta_end_edit_, 0, 1);
	end_layout->addWidget(phi_end_lable_, 1, 0);
	end_layout->addWidget(phi_end_edit_, 1, 1);
	end_box_ = new QGroupBox(QString::fromLocal8Bit("结束点"));
	end_box_->setLayout(end_layout);

	theta_gap_lable_ = new QLabel(QString::fromLocal8Bit("theta:"));
	theta_gap_edit_ = new QLineEdit("1.0");
	phi_gap_lable_ = new QLabel(QString::fromLocal8Bit("phi:"));
	phi_gap_edit_ = new QLineEdit("1.0");
	QGridLayout* gap_layout = new QGridLayout;
	gap_layout->addWidget(theta_gap_lable_, 0, 0);
	gap_layout->addWidget(theta_gap_edit_, 0, 1);
	gap_layout->addWidget(phi_gap_lable_, 1, 0);
	gap_layout->addWidget(phi_gap_edit_, 1, 1);
	gap_box_ = new QGroupBox(QString::fromLocal8Bit("点间距"));
	gap_box_->setLayout(gap_layout);

	theta_num_lable_ = new QLabel(QString::fromLocal8Bit("theta:"));
	theta_num_edit_ = new QLineEdit("181");
	theta_num_edit_->setEnabled(false);
	phi_num_lable_ = new QLabel(QString::fromLocal8Bit("phi:"));
	phi_num_edit_ = new QLineEdit("361");
	phi_num_edit_->setEnabled(false);
	QGridLayout* num_layout = new QGridLayout;
	num_layout->addWidget(theta_num_lable_, 0, 0);
	num_layout->addWidget(theta_num_edit_, 0, 1);
	num_layout->addWidget(phi_num_lable_, 1, 0);
	num_layout->addWidget(phi_num_edit_, 1, 1);
	num_box_ = new QGroupBox(QString::fromLocal8Bit("点个数"));
	num_box_->setLayout(num_layout);
	
	path_lable_ = new QLabel(QString::fromLocal8Bit("保存结果文件夹至:"));
	path_edit_ = new QLineEdit;
	browse_btn_ = new QPushButton(tr("Browse..."));
	connect(browse_btn_, SIGNAL(clicked()), this, SLOT(OnBrowseBtn()));

	QGridLayout * layout = new QGridLayout;
	//layout->addWidget(fre_lable_, 0, 0);
	layout->addWidget(path_lable_, 1, 0);
	layout->addWidget(path_edit_, 1, 1);
	layout->addWidget(browse_btn_, 1, 2);
	layout->addWidget(is_calc_nonrodome_lable_, 2, 0);
	layout->addWidget(yes_unit_, 2, 1);
	layout->addWidget(no_unit_, 2, 2);


	param_box_ = new QGroupBox(QString::fromLocal8Bit("仿真配置"));
	param_box_->setLayout(layout);

	QHBoxLayout * hlayout = new QHBoxLayout;
	hlayout->addSpacerItem(new QSpacerItem(20, 20, QSizePolicy::Expanding));
	hlayout->addWidget(ok_bt_);

	QVBoxLayout * Vlayout = new QVBoxLayout(this);
	Vlayout->addWidget(begin_box_);
	Vlayout->addWidget(end_box_);
	Vlayout->addWidget(gap_box_);
	// Vlayout->addWidget(num_box_);
	Vlayout->addWidget(param_box_);
	Vlayout->addLayout(hlayout);
}



void CalcConfigWidget::OnOKClicked() {
	if (!ReadTheta()) {
		return;
	}
	if (!ReadPhi()) {
		return;
	}

	PARSE_EDIT_LINE_TO_DOUBLE(fre_edit_, fre_);
	is_calc_nonrodome_ = (switch_group_button_->checkedId() == 0);
	polarization_type_ = polarization_type_combobox_->currentIndex() + 1;
	accept();
}


void CalcConfigWidget::OnThetaChange() {

}

void CalcConfigWidget::OnPhiChange() {

}


bool CalcConfigWidget::ReadTheta() {
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(theta_begin_edit_, min_theta_, -0.000001, 180.0000001);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(theta_end_edit_, max_theta_, -0.000001, 180.0000001);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(theta_gap_edit_, gap_theta_, -0.000001, 180.0000001);
	if (max_theta_ < min_theta_) {
		theta_begin_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		theta_end_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		return false;
	}
	num_theta_ = (max_theta_ - min_theta_) / gap_theta_ + 1;
}

bool CalcConfigWidget::ReadPhi() {
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(phi_begin_edit_, min_phi_, -0.000001, 360.0000001);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(phi_end_edit_, max_phi_, -0.000001, 360.0000001);
	PARSE_EDIT_LINE_TO_DOUBLE_RETURN(phi_gap_edit_, gap_phi_, -0.000001, 360.0000001);
	if (max_phi_ < min_phi_) {
		phi_begin_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		phi_end_edit_->setStyleSheet("background-color:rgba(255,0,0,255)");
		return false;
	}
	num_phi_ = (max_phi_ - min_phi_) / gap_phi_ + 1;
}

bool CalcConfigWidget::ReadPath() {

	// 打开文件夹选择对话框
	QString folderPath = QFileDialog::getExistingDirectory(this, tr("Select Folder"),
		QDir::homePath(),
		QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);

	if (!folderPath.isEmpty()) {
		// 输出选择的文件夹路径
		qDebug() << "Selected folder path: " << folderPath;
		// 这里可以添加你处理该文件夹路径的代码
		custom_path_ = folderPath.toStdString();
		path_edit_->setText(folderPath);
		return true;
	}
	return false;
}

void CalcConfigWidget::OnBrowseBtn() {
	ReadPath();
}