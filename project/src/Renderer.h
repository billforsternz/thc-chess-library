#ifndef RENDERER_H_
#define RENDERER_H_

#include <QtWidgets/QOpenGLWidget>
#include <QtGui/QPixmap>

class Renderer : public QOpenGLWidget
{
	Q_OBJECT

public:

	Renderer(QWidget* parent);
	void paintEvent(QPaintEvent* event);
	void onDrawBoard(const QString& str);

private:

	QPixmap* bmpWhiteKing;
	QPixmap* bmpWhiteQueen;
	QPixmap* bmpWhiteRook;
	QPixmap* bmpWhiteKnight;
	QPixmap* bmpWhiteBishop;
	QPixmap* bmpWhitePawn;
	QPixmap* bmpBlackKing;
	QPixmap* bmpBlackQueen;
	QPixmap* bmpBlackRook;
	QPixmap* bmpBlackKnight;
	QPixmap* bmpBlackBishop;
	QPixmap* bmpBlackPawn;

	std::map<std::string, QPixmap*> m_pieceTextures;

	QString m_board;

};
#endif
