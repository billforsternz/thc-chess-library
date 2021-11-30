
#include <QtWidgets/QApplication>
#include <QtWidgets/QStyleFactory>
#include "MainWindow.h"

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
	////QCoreApplication::setAttribute( Qt::AA_EnableHighDpiScaling, true );
	QApplication application(argc, argv);
	MainWindow mainWindow;
	mainWindow.show();
	return application.exec();
}
