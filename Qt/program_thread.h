#pragma once

#include <QtWidgets/QDialog>
#include <qcombobox.h>
#include <QLineEdit>
#include <QLabel>
#include <QHBoxLayout>
#include <QPushButton>
#include <QProgressDialog>
#include <QThread>
#include <functional>

class ProgramThreadFunc : public QThread
{
	Q_OBJECT
public:
	ProgramThreadFunc(QObject * parent = 0) {};
	void SetFunc(std::function<void()> func) {
		func_ = func;
	}
signals:
	void RunDone();
protected:
	void run() {
		func_();
		emit RunDone();
	}
	std::function<void()> func_;
};


class ProgramThread : public QThread
{
	Q_OBJECT
public:
	ProgramThread(QObject * parent = 0) {};
	void SetParam(int* step) {
		step_ = step;
	}
signals:
	void UpdateProgress(int nNum);
protected:
	void run() {
		int tmp = *step_;
		while (*step_ < 100) {
			if (tmp < (*step_ + 10)) tmp++;
			emit UpdateProgress(tmp);
			msleep(1000);
		}
	}
	int* step_;
};