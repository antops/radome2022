#pragma once

#include <map>
#include <QtWidgets/QDialog>
#include <QTabWidget>
#include <QGroupBox>
#include <QGridLayout>
#include <Qlabel>
#include <QLineEdit>
#include <QPushButton>
#include <QCloseEvent> 
#include "qcustomplot.h"
#include <QTextBrowser>
#include <QFile>
#include <QTextStream>
#include <QProcess>
#include <QProgressBar>

#include "../def.h"

class ProcessShowWidget : public QDialog
{
	Q_OBJECT

public:
	ProcessShowWidget(QWidget *parent = 0);
	~ProcessShowWidget();
	void SetProcess(QProcess* process) { process_ = process; }
	void Append(const std::string& msg);
	void QAppend(const QString& msg);
	bool IsCanceled() { return is_canceled_; }

	void SetWidgetText(QString str);

	void SetTextBrowserHidden(bool is_hidden);

	void FinishProcess();
//signals:
//	void UpdateFarField();
//
public slots :
	void OnReadoutput();
	void OnReadError();
	void OnCanceledBt();
private:
	

private:
	QLabel* label_;
	QTextBrowser * text_browser_;

	QProcess* process_;
	QProgressBar* progress_bar_;
	QPushButton* canceled_bt_;
	bool is_canceled_ = false;
};
