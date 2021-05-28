
#ifndef MAINWINDOW_H_
#define MAINWINDOW_H_

#include <QtWidgets/qmainwindow.h>
#include "Renderer.h"

namespace Ui
{
	class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT
public:
	MainWindow(QWidget* parent = 0);
	~MainWindow();

public slots:
	void onDraw();
	void onExit();

protected:
	void changeEvent(QEvent* e);

private:

	Ui::MainWindow* ui;
	Renderer* m_render;
};

#endif
