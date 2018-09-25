#include "boardsquare.h"

#include <QPen>
#include <QBrush>
#include <QDebug>
#include <QWidget>
#include <QPainter>
#include <QStyleOptionGraphicsItem>

#include "square.h"

using namespace Chess;

BoardSquare::BoardSquare(Board *board, Square square, const QSizeF &size, bool odd)
    : QGraphicsRectItem(square.file() * size.width(), (7 - square.rank()) * size.height(), size.width(), size.height()),
      m_board(board),
      m_square(square),
      m_size(size),
      m_odd(odd),
      m_squareType(odd ? Theme::Dark : Theme::Light)
{
    connect(this, SIGNAL(hoverEnter()), board, SLOT(hoverEnterSquare()));
    connect(this, SIGNAL(hoverLeave()), board, SLOT(hoverLeaveSquare()));
    connect(board->theme(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));

    QBrush brush = m_board->theme()->brushForSquare(m_squareType);
    setBrush(brush);
    setPen(Qt::NoPen);
    setZValue(1);
    setAcceptHoverEvents(true);
}

BoardSquare::~BoardSquare()
{
}

void BoardSquare::setSquareType(Theme::SquareType squareType)
{
    if (m_squareType == squareType)
        return;

    m_squareType = squareType;

    QBrush brush = m_board->theme()->brushForSquare(m_squareType);
    setBrush(brush);
}

void BoardSquare::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit hoverEnter();
    QGraphicsRectItem::hoverEnterEvent(event);
}

void BoardSquare::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit hoverLeave();
    QGraphicsRectItem::hoverLeaveEvent(event);
}

void BoardSquare::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);
/*    painter->setOpacity(0.5);*/
    painter->setPen(pen());
    QBrush b = brush();
    b.setTransform(painter->transform().inverted());
    painter->setBrush(b);
    painter->drawRect(rect());
}

void BoardSquare::themeChanged()
{
    QBrush brush = m_board->theme()->brushForSquare(m_squareType);
    setBrush(brush);
    update();
}
