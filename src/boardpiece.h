#ifndef BOARDPIECE_H
#define BOARDPIECE_H

#include <QGraphicsSvgItem>

#include "chess.h"
#include "piece.h"
#include "board.h"

class BoardPiece : public QGraphicsSvgItem {
    Q_OBJECT
public:
    BoardPiece(Board *board, Piece piece, const QSizeF &size);
    ~BoardPiece();

    Piece piece() const { return m_piece; }
    void setPiece(Piece piece);

    Square square() const { return m_piece.square(); }
    void setSquare(Square square);

Q_SIGNALS:
    void hoverEnter();
    void hoverLeave();

private Q_SLOTS:
    void themeChanged();

protected:
    virtual void hoverEnterEvent(QGraphicsSceneHoverEvent *event);
    virtual void hoverLeaveEvent(QGraphicsSceneHoverEvent *event);

private Q_SLOTS:
    void flip();

private:
    Piece m_piece;
    Board *m_board;
    QSizeF m_size;
};
#endif
