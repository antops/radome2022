#include "calc_config_widget.h"

#include <vtkjsoncpp/json/json.h>

#include "../def.h"

CalcConfigWidget::CalcConfigWidget(double fre_, QWidget *parent)
	: QDialog(parent)
{
	fre_lable_ = new QLabel(QString::fromLocal8Bit("Ƶ��(GHz):"));
	fre_edit_ = new QLineEdit(QString::number(fre_ / 1e9));

	polarization_type_lable_ = new QLabel(QString::fromLocal8Bit("��������:"));
	polarization_type_combobox_ = new QComboBox;
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("��ֱ����Ey"));
	polarization_type_combobox_->addItem(QString::fromLocal8Bit("ˮƽ����Ex"));

	is_calc_nonrodome_lable_ = new QLabel(QString::fromLocal8Bit("�Ƿ��������:"));
	
	switch_group_button_ = new QButtonGroup();
	yes_unit_ = new QRadioButton(QString::fromLocal8Bit("��"));
	no_unit_ = new QRadioButton(QString::fromLocal8Bit("��"));
	switch_group_button_->addButton(yes_unit_, 0);
	switch_group_button_->addButton(no_unit_, 1);
	yes_unit_->setChecked(true);
	no_unit_->setChecked(false);

	ok_bt_ = new QPushButton(QString::fromLocal8Bit("ȷ��"));
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
	begin_box_ = new QGroupBox(QString::fromLocal8Bit("��ʼ��"));
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
	end_box_ = new QGroupBox(QString::fromLocal8Bit("������"));
	end_box_->setLayout(end_layout);

	theta_gap_lable_ = new QLabel(QString::fromLocal8Bit("theta:"));
	theta_gap_edit_ = new QLineEdit("0.1");
	phi_gap_lable_ = new QLabel(QString::fromLocal8Bit("phi:"));
	phi_gap_edit_ = new QLineEdit("1.0");
	QGridLayout* gap_layout = new QGridLayout;
	gap_layout->addWidget(theta_gap_lable_, 0, 0);
	gap_layout->addWidget(theta_gap_edit_, 0, 1);
	gap_layout->addWidget(phi_gap_lable_, 1, 0);
	gap_layout->addWidget(phi_gap_edit_, 1, 1);
	gap_box_ = new QGroupBox(QString::fromLocal8Bit("����"));
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
	num_box_ = new QGroupBox(QString::fromLocal8Bit("�����"));
	num_box_->setLayout(num_layout);
	
	path_lable_ = new QLabel(QString::fromLocal8Bit("�������ļ�����:"));
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


	param_box_ = new QGroupBox(QString::fromLocal8Bit("��������"));
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

bool CalcConfigWidget::ReadPhi() {
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

bool CalcConfigWidget::ReadPath() {

	// ʹ�� QFileDialog ���û�ѡ���ļ���·��
	// QString selectedDir = QFileDialog::getExistingDirectory(nullptr, QString::fromLocal8Bit("ѡ��Ҫ�����ļ��е�λ��"), QDir::homePath());
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

	return false;
}

void CalcConfigWidget::OnBrowseBtn() {
	ReadPath();
}