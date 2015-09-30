#ifndef PIECE_H
#define PIECE_H

#include <QList>
#include <QString>

#include "chess.h"
#include "square.h"

class Piece {
public:
    Piece();
    Piece(Chess::Army army, Chess::PieceType piece, Square square);
    ~Piece();

    Chess::Army army() const { return m_army; }

    Chess::PieceType piece() const { return m_piece; }
    void setPiece(Chess::PieceType piece) { m_piece = piece; }

    Square square() const { return m_square; }
    void setSquare(Square square) { m_square = square; }

private:
    Chess::Army m_army;
    Chess::PieceType m_piece;
    Square m_square;
};

inline bool operator==(const Piece &a, const Piece &b)
{ return a.army() == b.army() && a.piece() == b.piece() && a.square() == b.square(); }

typedef QList<Piece> PieceList;

#endif
