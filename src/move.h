#ifndef MOVE_H
#define MOVE_H

#include <QList>
#include <QString>

#include "chess.h"
#include "square.h"

class Move {
public:
    Move();
    ~Move();

    bool isValid() const;

    Chess::PieceType piece() const { return m_piece; }
    void setPiece(Chess::PieceType piece) { m_piece = piece; }

    Square start() const { return m_start; }
    void setStart(Square start) { m_start = start; }

    Square end() const { return m_end; }
    void setEnd(Square end) { m_end = end; }

    bool isCastle() const { return m_isCastle; }
    void setCastle(bool isCastle) { m_isCastle = isCastle; }

    bool isKingSideCastle() const { return m_castle == Chess::KingSide; }
    bool isQueenSideCastle() const { return m_castle == Chess::QueenSide; }
    void setCastleSide(Chess::Castle castle) { m_castle = castle; }

    int fileOfDeparture() const { return start().isValid() ? start().file() : m_fileOfDeparture; }
    void setFileOfDeparture(int fileOfDeparture) { m_fileOfDeparture = fileOfDeparture; }

    int rankOfDeparture() const { return start().isValid() ? start().rank() : m_rankOfDeparture; }
    void setRankOfDeparture(int rankOfDeparture) { m_rankOfDeparture = rankOfDeparture; }

    Chess::PieceType promotion() const { return m_promotion; }
    void setPromotion(Chess::PieceType promotion) { m_promotion = promotion; }

    bool isCapture() const { return m_isCapture; }
    void setCapture(bool isCapture) { m_isCapture = isCapture; }

    bool isCheck() const { return m_isCheck; }
    void setCheck(bool isCheck) { m_isCheck = isCheck; }

    bool isCheckMate() const { return m_isCheckMate; }
    void setCheckMate(bool isCheckMate) { m_isCheckMate = isCheckMate; }

    bool isEnPassant() const { return m_isEnPassant; }
    void setEnPassant(bool isEnPassant) { m_isEnPassant = isEnPassant; }

    bool isDrawOfferred() const { return m_isDrawOffered; }
    void setDrawOfferred(bool isDrawOfferred) { m_isDrawOffered = isDrawOfferred; }

private:
    Chess::PieceType m_piece;

    Square m_start;
    Square m_end;

    bool m_isCastle;
    Chess::Castle m_castle;

    int m_fileOfDeparture;
    int m_rankOfDeparture;

    Chess::PieceType m_promotion;

    bool m_isCapture;
    bool m_isCheck;
    bool m_isCheckMate;
    bool m_isEnPassant;
    bool m_isDrawOffered;
};

inline bool operator==(const Move &a, const Move &b) { return a.end() == b.end(); }

typedef QList<Move> MoveList;

#endif
