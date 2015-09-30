#include "move.h"

using namespace Chess;

Move::Move()
    : m_piece(Unknown),
      m_start(Square()),
      m_end(Square()),
      m_isCastle(false),
      m_castle(KingSide),
      m_fileOfDeparture(-1),
      m_rankOfDeparture(-1),
      m_promotion(Unknown),
      m_isCapture(false),
      m_isCheck(false),
      m_isCheckMate(false),
      m_isEnPassant(false),
      m_isDrawOffered(false)
{
}

Move::~Move()
{
}

bool Move::isValid() const
{
    return (m_piece != Unknown || m_start.isValid()) && m_end.isValid();
}
