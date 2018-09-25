#include "piece.h"

#include <QDebug>

using namespace Chess;

Piece::Piece()
    : m_army(White),
      m_piece(Unknown),
      m_square(Square())
{
}

Piece::Piece(Chess::Army army, Chess::PieceType piece, Square square)
    : m_army(army),
      m_piece(piece),
      m_square(square)
{
}

Piece::~Piece()
{
}
