
#include <QtCore/QTimer>
#include "MainWindow.h"
#include "ui_MainWindow.h"
#include <deque>
#include <set>

#include "./../../src/ChessPosition.h"
#include "./../../src/ChessRules.h"

class Node
{
public:
	thc::ChessPosition pos;
	int depth;
	Node* parent;
	std::vector<Node*> children;
	enum class States
	{
		UNDEF = 0,
		WHITE = 1,
		BLACK = -1
	};

	States state;

	Node(std::string initPos, int depth)
	{
		pos.Forsyth(initPos.c_str());
		this->depth = depth;
		parent = nullptr;
		state = States::UNDEF;
	}

	static void onWhiteMate(Node *node)
	{
		if (!node->pos.WhiteToPlay())
		{
			if (node->state == States::WHITE)
			{
				node->parent->state = States::WHITE; // black to play always has parent node
			}
			bool allChildrenWhite = true;
			for (int k = 0; k < node->children.size(); k++)
			{
				if (node->children[k]->state != States::WHITE)
				{
					allChildrenWhite = false;
					break;
				}
			}
			if (allChildrenWhite)
			{
				node->state = States::WHITE;
				node->parent->state = States::WHITE; // black to play always has parent node
			}
		}
		if (node->parent != nullptr)
		{
			onWhiteMate(node->parent);
		}
	}

	void print()
	{
		printf("[%d] %s\n", depth, pos.ForsythPublish().c_str());
	}

	void printTree()
	{
		Node* node = this;
		while (node != nullptr)
		{
			printf("[%d] %s\n", node->depth, node->pos.ForsythPublish().c_str());
			node = node->parent;
		}
	}

	static void printWin(Node* node)
	{
		while (node != nullptr)
		{
			if (node->state == States::WHITE)
			{
				printf("[%d] %s\n", node->depth, node->pos.ForsythPublish().c_str());
			}
			Node* dest = nullptr;
			for (int k = 0; k < node->children.size(); k++)
			{
				if (node->children[k]->state == States::WHITE)
				{
					if (node->pos.WhiteToPlay())
					{
						dest = node->children[k];
						break;
					}
					else
					{
						printWin(node->children[k]);
					}
				}
			}
			node = dest;
		}
	}

	void addChild(Node * child)
	{
		children.push_back(child);
	}
};

void findWhiteMate(std::string initPos, int moves)
{
	std::vector<Node*> nodes;
	std::deque<Node*> tree;

	int maxDepth = 2 * moves - 1;
	Node *init = new Node(initPos, 0);
	nodes.push_back(init);
	tree.push_back(init);
	bool whiteWin = false;

	while (!tree.empty() && (init->state != Node::States::WHITE))
	{
		Node *node = tree.front();
		//node->print();
		if (node->depth < maxDepth)
		{
			thc::ChessRules cr = node->pos;
			std::vector<thc::Move> moves;
			std::vector<bool> check;
			std::vector<bool> mate;
			std::vector<bool> stalemate;
			cr.GenLegalMoveList(moves, check, mate, stalemate);

			for (unsigned int i = 0; i < moves.size(); i++)
			{
				thc::Move mv = moves[i];
				cr.PushMove(mv);

				std::string str = cr.ForsythPublish();

				Node *child = new Node(str, node->depth + 1);
				child->parent = node;
				nodes.push_back(child);
				tree.push_back(child);
				node->addChild(child);

				if (mate[i])
				{
					child->state = child->pos.WhiteToPlay() ? Node::States::BLACK : Node::States::WHITE;
					Node::onWhiteMate(child);
				}
				cr.PopMove(mv);
			}
		}
		tree.pop_front();
	}
	printf("-- MATE --\n");
	Node::printWin(init);
}

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_render = new Renderer(ui->widget);

	//// Checkmate in 3
	//std::string pos = "r5rk/5p1p/5R2/4B3/8/8/7P/7K";
	std::string pos = "8/pk1B4/p7/2K1p3/8/8/4Q3/8";

	//// Checkmate in 2
	//std::string pos = "7Q/2Rp4/2pN4/p2rp3/P2N4/B1k5/2PpRb2/3K2n1";

	//// Checkmate in 2 https://www.chess.com/blog/ThePawnSlayer/checkmate-in-two-puzzles-test-very-hard
	////std::string pos = "kbK5/pp6/1P6/8/8/8/8/R7";
	//std::string pos = "2b3N1/8/1r2pN1b/1p2kp2/1P1R4/8/4K3/6Q1";

	//std::string pos = "1nK1R3/5Bpb/3k4/N1b2pB1/3pp3/5r2/8/2n5";

	//// Checkmate in 1
	//std::string pos = "3b4/R3P3/2kPR1n1/P1p1n3/4K1p1/2Nr4/5p2/1b6";

	//// Play chess with God https://timkr.home.xs4all.nl/chess2/diary_3.htm
	//std::string pos = "1N6/1RK5/5n2/8/8/8/5n2/6k1";

	ui->txtData->setText(QString::fromStdString(pos));

	thc::ChessPosition chessPos;
	std::string fen = pos + " w - - 0 1";
	if (!chessPos.Forsyth(fen.c_str()))
		printf("[ERROR] invalid FEN: %s\n", pos.c_str());
	else
		findWhiteMate(fen, 3);

	connect(ui->btnDraw, SIGNAL(clicked()), this, SLOT(onDraw()));
	connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(onExit()));

	onDraw();

	ui->txtData->setText(pos.c_str());
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
