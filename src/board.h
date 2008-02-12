#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include <QList>

#include "game.h"

class Theme;
class Piece;
class BoardSquare;
class BoardPiece;
class QPainter;
class QWidget;
class QStyleOptionGraphicsItem;

class Borders;
class Board : public QGraphicsScene {
    Q_OBJECT
public:
    Board(Game *game);
    ~Board();

    Game *game() const { return qobject_cast<Game*>(parent()); }
    Theme *theme() const { return m_theme; }

    Chess::Army armyInFront() const { return m_armyInFront; }
    void setArmyInFront(Chess::Army army);

public Q_SLOTS:
    void flipBoard();

Q_SIGNALS:
    void boardFlipped();
    void hoverEnter();
    void hoverLeave();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);

private Q_SLOTS:
    void clearBoard();
    void clearSquares();
    void resetBoard();
    void hoverEnterSquare();
    void hoverLeaveSquare();
    void hoverEnterPiece();
    void hoverLeavePiece();

private:
    Theme *m_theme;
    Borders *m_borders;
    QHash<int, BoardSquare*> m_squares;
    BoardPiece *m_currentPiece;
    Square m_currentSquare;
    Chess::Army m_armyInFront;
    friend class Borders;
};

class Borders : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    Borders(Board *board);
    ~Borders();

protected:
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget = 0);

private Q_SLOTS:
    void flip();

private:
    QTransform textTransform(QPainter *painter, const QChar &c, const QRectF &r) const;

private:
    Board *m_board;
};

#endif
