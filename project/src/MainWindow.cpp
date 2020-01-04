
#include <QtCore/QTimer>
#include "MainWindow.h"
#include "ui_MainWindow.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_render = new Renderer(ui->widget);

	// Checkmate in 2
	ui->txtData->setText("7Q/2Rp4/2pN4/p2rp3/P2N4/B1k5/2PpRb2/3K2n1");

	connect(ui->btnDraw, SIGNAL(clicked()), this, SLOT(onDraw()));
	connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(onExit()));

	onDraw();

	ui->txtData->setText("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR");
}

MainWindow::~MainWindow()
{
	delete ui;
	delete m_render;
}

void MainWindow::changeEvent(QEvent* e)
{
	QMainWindow::changeEvent(e);
	switch (e->type())
	{
	case QEvent::LanguageChange:
		ui->retranslateUi(this);
		break;
	default:
		break;
	}
}

void MainWindow::onDraw()
{
	m_render->onDrawBoard(ui->txtData->text());
}

void MainWindow::onExit()
{
	QWidget::close();
}
