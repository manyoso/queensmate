#ifndef BOARDSQUARE_H
#define BOARDSQUARE_H

#include <QGraphicsRectItem>

#include "theme.h"
#include "board.h"

class Square;

class BoardSquare : public QObject, public QGraphicsRectItem {
    Q_OBJECT
public:
    BoardSquare(Board *board, Square square, const QSizeF &size, bool odd);
    ~BoardSquare();

    Square square() const { return m_square; }

    bool isOdd() const { return m_odd; }

    Theme::SquareType squareType() const { return m_squareType; }
    void setSquareType(Theme::SquareType squareType);

Q_SIGNALS:
    void hoverEnter();
    void hoverLeave();

private Q_SLOTS:
    void themeChanged();

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);
    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

private:
    Board *m_board;
    Square m_square;
    QSizeF m_size;
    bool m_odd;
    Theme::SquareType m_squareType;
};

#endif
