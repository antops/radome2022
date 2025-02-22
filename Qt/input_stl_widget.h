#pragma once

#include <QtWidgets/QDialog>
#include <qcombobox.h>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QComboBox>

class InputStlWidget : public QDialog
{
	Q_OBJECT
public:
	InputStlWidget(int paramCount, QWidget *parent = 0);
	~InputStlWidget();

	const std::vector<std::string>& GetResults() const {
		return results_;
	}

	const std::vector<std::string>& GetSurfaceResults() const {
		return surface_results_;
	}

private slots:
    
	void on_ok_btn();
	void OnBrowseBtn();

private:
	void GenMeta();
	void GenSurface();

private:
	QLabel *mesh_scale_label_;
	QPushButton * okBtn;

	std::vector<QLineEdit*> file_name_line_vec_;
	std::vector<QPushButton*> btn_vec_;
	std::vector<std::string> results_;
	std::vector<std::string> surface_results_;
	std::string dir_path_;

};

class InputStlNumberWidget : public QDialog
{
	Q_OBJECT
public:
	InputStlNumberWidget(QWidget* parent = 0) {
		QVBoxLayout* subLayout = new QVBoxLayout(this);

		QLabel* label1 = new QLabel(QString::fromLocal8Bit("需要导入的stl个数"));
		fileNameEdit = new QLineEdit("0");
		QPushButton* okBtn = new QPushButton(QString::fromLocal8Bit("确认"));
		subLayout->addWidget(label1);
		subLayout->addWidget(fileNameEdit);
		subLayout->addWidget(okBtn);
		connect(okBtn, SIGNAL(clicked()), this, SLOT(on_ok_btn()));
	}
	~InputStlNumberWidget() {}

	int GetNumber() const {
		return number;
	}

private slots:

	void on_ok_btn() {
		bool ok;
		number = fileNameEdit->text().toInt(&ok);
		if (!ok) {
			number = 0;
			return;  // 如果输入不是合法的整数，直接返回
		}
		accept();
	}
private:
	QLineEdit* fileNameEdit;
	int number = 0;
};
