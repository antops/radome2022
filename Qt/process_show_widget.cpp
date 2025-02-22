#include "process_show_widget.h"

#include "../def.h"

ProcessShowWidget::ProcessShowWidget( QWidget *parent)
	: QDialog(parent)
{
	label_ = new QLabel;
	QVBoxLayout *layout = new QVBoxLayout(this);
	text_browser_ = new QTextBrowser();
	text_browser_->setMinimumWidth(500);

	progress_bar_ = new QProgressBar;
	progress_bar_->setMinimum(0);
	progress_bar_->setMaximum(0);
	progress_bar_->setMinimumWidth(500);

	QHBoxLayout *hlayout = new QHBoxLayout();
	canceled_bt_ = new QPushButton(QString::fromLocal8Bit("取消任务"));
	canceled_bt_->setMaximumWidth(200);
	connect(canceled_bt_, SIGNAL(clicked()), this, SLOT(OnCanceledBt()));
	hlayout->addSpacing(0);
	hlayout->addWidget(canceled_bt_);

	layout->addWidget(label_);
	layout->addWidget(progress_bar_);
	layout->addWidget(text_browser_);
	layout->addLayout(hlayout);
}

void ProcessShowWidget::FinishProcess() {
	canceled_bt_->setText(QString::fromLocal8Bit("确定"));
}

ProcessShowWidget::~ProcessShowWidget()
{
}

void ProcessShowWidget::SetWidgetText(QString str) {
	label_->setText(str);
}

void ProcessShowWidget::SetTextBrowserHidden(bool is_hidden) {
	text_browser_->setHidden(is_hidden);
}

void ProcessShowWidget::OnReadoutput() {
	text_browser_->append(QString::fromLocal8Bit(process_->readAllStandardOutput().data()));
}

void ProcessShowWidget::OnReadError() {
	text_browser_->append(QString::fromLocal8Bit(process_->readAllStandardError().data()));
}

void ProcessShowWidget::OnCanceledBt()
{
	is_canceled_ = true;
	close();
}

void ProcessShowWidget::Append(const std::string& msg) {
	text_browser_->append(QString::fromStdString(msg));
}

void ProcessShowWidget::QAppend(const QString & msg)
{
	text_browser_->append(msg);
}
