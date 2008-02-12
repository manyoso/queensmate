#ifndef NOTATION_H
#define NOTATION_H

#include <QString>

#include "chess.h"

class Move;
class Square;

/* TODO
 * Draw offer...
 * Check...
 * CheckMate...
 * Promotion...
 * Castling...
 * En Passant...
 * Disambiguation eg, FileOfDeparture, RankOfDeparture, etc...
 * Error handling and messages
 */

class Notation {
public:
    static Move stringToMove(const QString &string, Chess::NotationType notation = Chess::Standard, bool *ok = 0, QString *err = 0);
    static QString moveToString(Move move, Chess::NotationType notation = Chess::Standard);

    static Square stringToSquare(const QString &string, Chess::NotationType notation = Chess::Standard, bool *ok = 0, QString *err = 0);
    static QString squareToString(Square square, Chess::NotationType notation = Chess::Standard);

    static Chess::PieceType charToPiece(const QChar &ch, Chess::NotationType notation = Chess::Standard, bool *ok = 0, QString *err = 0);
    static QChar pieceToChar(Chess::PieceType piece, Chess::NotationType notation = Chess::Standard);

private:
    Notation();
    ~Notation();
};

#endif
