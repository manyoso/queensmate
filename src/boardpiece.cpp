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
    scale(size.width() / rect.width(), size.height() / rect.width());

    setZValue(2);
    setFlag(ItemIsMovable, true);
    setAcceptsHoverEvents(true);
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
    scale(m_size.width() / rect.width(), m_size.height() / rect.width());
    setVisible(true);
    update();
}
