#ifndef BITBOARD_H
#define BITBOARD_H

#include <QBitArray>

#include <QHash>

#include "square.h"

class BitBoard : public QBitArray {
public:
    BitBoard();
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

#endif
