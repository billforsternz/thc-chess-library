
#include <QtCore/QTimer>
#include "MainWindow.h"
#include "ui_MainWindow.h"

#include "./../../src/ChessPosition.h"
#include "./../../src/ChessRules.h"

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_render = new Renderer(ui->widget);

	//// Checkmate in 2
	//std::string pos = "7Q/2Rp4/2pN4/p2rp3/P2N4/B1k5/2PpRb2/3K2n1";

	// Play chess with God https://timkr.home.xs4all.nl/chess2/diary_3.htm
	std::string pos = "1N6/1RK5/5n2/8/8/8/5n2/6k1";

	ui->txtData->setText(QString::fromStdString(pos));

	thc::ChessPosition chessPos;
	pos += " w - - 0 1";
	bool ok = chessPos.Forsyth(pos.c_str());
	pos = chessPos.ToDebugStr();

	thc::ChessRules cr = chessPos;
	thc::Move mv;
	printf("List of all legal moves in the current position\n");
	std::vector<thc::Move> moves;
	std::vector<bool> check;
	std::vector<bool> mate;
	std::vector<bool> stalemate;
	cr.GenLegalMoveList(moves, check, mate, stalemate);
	unsigned int len = moves.size();
	for (unsigned int i = 0; i < len; i++)
	{
		mv = moves[i];
		std::string mv_txt = mv.NaturalOut(&cr);
		const char* suffix = "";
		if (check[i])
			suffix = " (note '+' indicates check)";
		else if (mate[i])
			suffix = " (note '#' indicates mate)";
		printf("4.%s%s\n", mv_txt.c_str(), suffix);
	}

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
