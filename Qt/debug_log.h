#pragma once

#include <QtGlobal>
#include <QFile>
#include <QDir>
#include <QDate>
#include <QTime>
#include <qt_windows.h>

class DebugLog
{
public:
	static void RecordLog();
private:
	static void Log(QtMsgType type, const QMessageLogContext &context, const QString &msg);
};
