
#include "Renderer.h"

#include <QtGui/QPainter>

Renderer::Renderer(QWidget* parent) : QOpenGLWidget(parent)
{
	setAutoFillBackground(false);
	setGeometry(0, 0, parent->width(), parent->height());

	bmpWhiteKing = new QPixmap(":/img/white_king.png");
	bmpWhiteQueen = new QPixmap(":/img/white_queen.png");
	bmpWhiteRook = new QPixmap(":/img/white_rook.png");
	bmpWhiteKnight = new QPixmap(":/img/white_knight.png");
	bmpWhiteBishop = new QPixmap(":/img/white_bishop.png");
	bmpWhitePawn = new QPixmap(":/img/white_pawn.png");
	bmpBlackKing = new QPixmap(":/img/black_king.png");
	bmpBlackQueen = new QPixmap(":/img/black_queen.png");
	bmpBlackRook = new QPixmap(":/img/black_rook.png");
	bmpBlackKnight = new QPixmap(":/img/black_knight.png");
	bmpBlackBishop = new QPixmap(":/img/black_bishop.png");
	bmpBlackPawn = new QPixmap(":/img/black_pawn.png");

	m_pieceTextures =
	{
		{ "q", bmpBlackQueen },
		{ "k", bmpBlackKing },
		{ "b", bmpBlackBishop },
		{ "n", bmpBlackKnight },
		{ "r", bmpBlackRook },
		{ "p", bmpBlackPawn },
		{ "Q", bmpWhiteQueen },
		{ "K", bmpWhiteKing },
		{ "B", bmpWhiteBishop },
		{ "N", bmpWhiteKnight },
		{ "R", bmpWhiteRook },
		{ "P", bmpWhitePawn }
	};
}

void Renderer::paintEvent(QPaintEvent* event)
{
	onDrawBoard(m_board);
}

void Renderer::onDrawBoard(const QString& board)
{
	m_board = board;

	QPainter painter;
	painter.begin(this);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.fillRect(geometry(), QColor(16, 19, 54, 255));

	int cellSize = geometry().width() < geometry().height() ? geometry().width() / 8
		: geometry().height() / 8;
	for (int y = 0; y < 8; y++)
	{
		for (int x = 0; x < 8; x++)
		{
			if ((y + x) % 2 == 0)
			{
				painter.fillRect(QRect(x * cellSize, y * cellSize, cellSize, cellSize),
					QColor(54, 40, 117, 255));
			}
		}
	}

	int row = 0;
	int column = 0;
	bool isLineComplete = false;
	std::string initPosition = board.toStdString();

	for (int charIndex = 0; charIndex < initPosition.length(); ++charIndex)
	{
		std::string positionValue = initPosition.substr(charIndex, 1);

		if (!isLineComplete)
		{
			if (m_pieceTextures.count(positionValue))
			{
				painter.drawPixmap(column * cellSize, row * cellSize, cellSize,
					cellSize, *m_pieceTextures[positionValue]);
				column++;
			}
			else
			{
				column += std::atoi(positionValue.c_str());
			}
			if (column >= 8)
			{
				isLineComplete = true;
			}
		}
		else
		{
			if (positionValue == "/")
			{
				row++;
				column = 0;
				isLineComplete = false;
			}
		}
	}
	painter.end();
}
