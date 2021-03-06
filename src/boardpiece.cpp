#include "boardpiece.h"

#include <QColor>
#include <QDebug>
#include <QPainter>
#include <QSvgRenderer>

#include "theme.h"
#include "notation.h"

using namespace Chess;

BoardPiece::BoardPiece(Board *board, Piece piece, const QSizeF &size)
    : QGraphicsSvgItem(),
      m_board(board),
      m_size(size)
{
    connect(this, SIGNAL(hoverEnter()), board, SLOT(hoverEnterPiece()));
    connect(this, SIGNAL(hoverLeave()), board, SLOT(hoverLeavePiece()));
    connect(board, SIGNAL(boardFlipped()), this, SLOT(flip()));
    connect(board->theme(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));

    setPiece(piece);

    QRectF rect = boundingRect();
    setTransform(QTransform::fromScale(size.width() / rect.width(),
        size.height() / rect.width()), true);

    setZValue(2);
    setFlag(ItemIsMovable, true);
    setAcceptHoverEvents(true);
}

BoardPiece::~BoardPiece()
{
}

void BoardPiece::setPiece(Piece piece)
{
    if (m_piece == piece)
        return;

    m_piece = piece;
    setSharedRenderer(m_board->theme()->rendererForPiece(m_piece.army(), m_piece.piece()));
    setSquare(m_piece.square());
}

void BoardPiece::setSquare(Square square)
{
    setPos((m_board->armyInFront() == White ? square.file() : (7 - square.file())) * m_size.width(),
           (m_board->armyInFront() == White ? (7 - square.rank()) : square.rank()) * m_size.height());
}

QVariant BoardPiece::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if (change == ItemPositionChange && scene()) {
        QPointF pos = value.toPointF();
        QRectF rect = m_board->boardRect();
        QRectF bound = QRectF(pos, QSizeF(m_size.width(), m_size.height()));

        if (!rect.contains(bound)) {
            pos.setX(qMin(rect.right() - m_size.width(), qMax(pos.x(), rect.left())));
            pos.setY(qMin(rect.bottom() - m_size.height(), qMax(pos.y(), rect.top())));
            return pos;
        }
    }
    return QGraphicsItem::itemChange(change, value);
}

void BoardPiece::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    emit hoverEnter();
    QGraphicsSvgItem::hoverEnterEvent(event);
}

void BoardPiece::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    emit hoverLeave();
    QGraphicsSvgItem::hoverLeaveEvent(event);
}

void BoardPiece::flip()
{
    setSquare(m_piece.square());
}

void BoardPiece::themeChanged()
{
    setVisible(false);
    resetTransform();
    setSharedRenderer(m_board->theme()->rendererForPiece(m_piece.army(), m_piece.piece()));
    QRectF rect = boundingRect();
        setTransform(QTransform::fromScale(m_size.width() / rect.width(),
        m_size.height() / rect.width()), true);
    setVisible(true);
    update();
}
