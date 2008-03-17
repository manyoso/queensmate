#ifndef BITBOARD_H
#define BITBOARD_H

#include <QBitArray>

#include <QDebug>

#include "square.h"

class BitBoard : public QBitArray {
public:
    BitBoard();
    BitBoard(const Square &square);
    BitBoard(const SquareList &squareList);
    BitBoard(const QBitArray &other);
    ~BitBoard();

    bool isClear() const;

    bool isSquareOccupied(Square square) const;

    SquareList occupiedSquares() const;

    void setBoard(const SquareList &squareList);
    void setSquare(Square square);

    Square bitToSquare(int bit) const;
    int squareToBit(Square square) const;
};

QDebug operator<<(QDebug, const BitBoard &);

#endif
