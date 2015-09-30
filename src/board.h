#ifndef BOARD_H
#define BOARD_H

#include <QGraphicsScene>
#include <QGraphicsRectItem>

#include <QList>

#include "game.h"
#include "theme.h"

class Piece;
class BitBoard;
class BoardPiece;
class BoardSquare;
class QWidget;
class QPainter;
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

    QRectF boardRect() const;

    bool isMovesShown() const { return m_isMovesShown; }
    bool isAttacksShown() const { return m_isAttacksShown; }
    bool isDefendsShown() const { return m_isDefendsShown; }
    bool isAttackedByShown() const { return m_isAttackedByShown; }
    bool isDefendedByShown() const { return m_isDefendedByShown; }

public Q_SLOTS:
    void flipBoard();
    void setMovesShown(bool checked) { m_isMovesShown = checked; resetSquares(); }
    void setAttacksShown(bool checked) { m_isAttacksShown = checked; resetSquares(); }
    void setDefendsShown(bool checked) { m_isDefendsShown = checked; resetSquares(); }
    void setAttackedByShown(bool checked) { m_isAttackedByShown = checked; resetSquares(); }
    void setDefendedByShown(bool checked) { m_isDefendedByShown = checked; resetSquares(); }

Q_SIGNALS:
    void boardFlipped();
    void hoverEnter();
    void hoverLeave();
    void currentPieceChanged();
    void currentSquareChanged();

protected:
    virtual void keyPressEvent(QKeyEvent *event);
    virtual void keyReleaseEvent(QKeyEvent *event);
    virtual void mousePressEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseMoveEvent(QGraphicsSceneMouseEvent *event);
    virtual void mouseReleaseEvent(QGraphicsSceneMouseEvent *event);
    virtual void contextMenuEvent(QGraphicsSceneContextMenuEvent *event);

private Q_SLOTS:
    void clearBoard();
    void clearSquares();
    void resetBoard();
    void resetSquares();
    void hoverEnterSquare();
    void hoverLeaveSquare();
    void hoverEnterPiece();
    void hoverLeavePiece();
    void changeTheme();

private:
    void colorBoard(Theme::SquareType type, const BitBoard &board);

private:
    Theme *m_theme;
    Borders *m_borders;
    QHash<int, BoardSquare*> m_squares;
    BoardPiece *m_currentPiece;
    Square m_currentSquare;
    Chess::Army m_armyInFront;
    bool m_isMovesShown, m_isAttacksShown, m_isDefendsShown, m_isAttackedByShown, m_isDefendedByShown;
    QHash<Square, QColor> m_squareBorders;
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
