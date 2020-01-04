
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include "MainWindow.h"

#include "./../../src/ChessEngine.h"
void ReportOnProgress(
	bool    init,
	int     multipv,
	std::vector<thc::Move>& pv,
	int     score_cp,
	int     depth
) {}

#if _DEBUG
#pragma comment( lib, "Qt5Cored.lib" )
#pragma comment( lib, "Qt5Guid.lib" )
#pragma comment( lib, "Qt5Widgetsd.lib" )
#else
#pragma comment( lib, "Qt5Core.lib" )
#pragma comment( lib, "Qt5Gui.lib" )
#pragma comment( lib, "Qt5Widgets.lib" )
#endif

int main(int argc, char* argv[])
{
	QApplication::setStyle(QStyleFactory::create("Fusion"));
	QApplication application(argc, argv);
	MainWindow mainWindow;
	mainWindow.show();
	return application.exec();
}
