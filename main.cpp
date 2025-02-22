#define vtkRenderingCore_AUTOINIT 4(vtkInteractionStyle,vtkRenderingFreeType,vtkRenderingFreeType,vtkRenderingOpenGL2)
#define vtkRenderingVolume_AUTOINIT 1(vtkRenderingVolumeOpenGL2)
#include "radome.h"
#include <QtWidgets/QApplication>
#include "vtkOutputWindow.h"
#include "Qt/debug_log.h"

int main(int argc, char *argv[])
{
	vtkOutputWindow::SetGlobalWarningDisplay(0);
	QApplication a(argc, argv);
	QDateTime dateTime = QDateTime::currentDateTime();
	QDateTime expirationDate = QDateTime(QDate(2026, 1, 1), QTime(0, 0, 0));
	if (expirationDate < dateTime) {
		return -1;
	}
	Radome w;
	DebugLog::RecordLog();
	w.show();
	return a.exec();
}
