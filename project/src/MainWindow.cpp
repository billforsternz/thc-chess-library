
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
	int state; // 0: undefined, 1: white mate, -1: black mate

	Node(std::string initPos, int depth)
	{
		pos.Forsyth(initPos.c_str());
		this->depth = depth;
		parent = nullptr;
		state = 0;
	}

	static void onWhiteMate(Node *node)
	{
		if (!node->pos.WhiteToPlay())
		{
			if (node->state == 1)
			{
				node->parent->state = 1; // black to play always has parent node
			}
			bool allChildrenWhite = true;
			for (int k = 0; k < node->children.size(); k++)
			{
				if (node->children[k]->state != 1)
				{
					allChildrenWhite = false;
					break;
				}
			}
			if (allChildrenWhite)
			{
				node->state = 1;
				node->parent->state = 1; // black to play always has parent node
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
			if (node->state == 1)
			{
				printf("[%d] %s\n", node->depth, node->pos.ForsythPublish().c_str());
			}
			Node* dest = nullptr;
			for (int k = 0; k < node->children.size(); k++)
			{
				if (node->children[k]->state == 1)
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

void process(std::string initPos, int maxDepth)
{
	std::vector<Node*> nodes;
	std::deque<Node*> tree;
	//std::set<uint64_t> discovered;//? Hash64Calculate doesn't seem to be useful to check duplicated game states

	Node *init = new Node(initPos, 0);
	nodes.push_back(init);
	tree.push_back(init);
	//discovered.insert(init->pos.Hash64Calculate());
	bool whiteWin = false;

	while (!tree.empty() && (init->state != 1))
	{
		Node *node = tree.front();
		//node.print();
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

				//if (discovered.count(cr.Hash64Calculate()))
				//{
				//	//printf("found: %s\n", str.c_str());
				//}
				//else
				{
					Node *child = new Node(str, node->depth + 1);
					child->parent = node;
					nodes.push_back(child);
					tree.push_back(child);
					node->addChild(child);
					//discovered.insert(child->pos.Hash64Calculate());

					if (mate[i])
					{
						//child->printTree();
						child->state = child->pos.WhiteToPlay() ? -1 : 1;
						Node::onWhiteMate(child);
					}
				}

				cr.PopMove(mv);
			}
		}
		tree.pop_front();
	}
	Node::printWin(init);
}

MainWindow::MainWindow(QWidget* parent)
	: QMainWindow(parent), ui(new Ui::MainWindow)
{
	ui->setupUi(this);

	m_render = new Renderer(ui->widget);

	//// Checkmate in 2
	//std::string pos = "7Q/2Rp4/2pN4/p2rp3/P2N4/B1k5/2PpRb2/3K2n1";

	// Checkmate in 2 https://www.chess.com/blog/ThePawnSlayer/checkmate-in-two-puzzles-test-very-hard
	//std::string pos = "kbK5/pp6/1P6/8/8/8/8/R7";
	//std::string pos = "2b3N1/8/1r2pN1b/1p2kp2/1P1R4/8/4K3/6Q1";

	std::string pos = "1nK1R3/5Bpb/3k4/N1b2pB1/3pp3/5r2/8/2n5";

	//// Checkmate in 1
	//std::string pos = "3b4/R3P3/2kPR1n1/P1p1n3/4K1p1/2Nr4/5p2/1b6";

	//// Play chess with God https://timkr.home.xs4all.nl/chess2/diary_3.htm
	//std::string pos = "1N6/1RK5/5n2/8/8/8/5n2/6k1";

	ui->txtData->setText(QString::fromStdString(pos));

	thc::ChessPosition chessPos;
	pos += " w - - 0 1";
	bool ok = chessPos.Forsyth(pos.c_str());

	process(pos, 3);

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
		//std::string mv_txt = mv.NaturalOut(&cr);
		std::string mv_txt = mv.TerseOut();
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
