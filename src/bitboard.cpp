#include "bitboard.h"

#include <QDebug>

#include "notation.h"

BitBoard::BitBoard()
    : QBitArray(64)
{
}

BitBoard::BitBoard(const Square &square)
    : QBitArray(64)
{
    setSquare(square);
}

BitBoard::BitBoard(const SquareList &squareList)
    : QBitArray(64)
{
    setBoard(squareList);
}

BitBoard::BitBoard(const QBitArray &other)
    : QBitArray(other)
{
}

BitBoard::~BitBoard()
{
}

bool BitBoard::isClear() const
{
    return count(true) == 0;
}

bool BitBoard::isSquareOccupied(Square square) const
{
    int bit = squareToBit(square);
    return testBit(bit);
}

SquareList BitBoard::occupiedSquares() const
{
    SquareList occupiedList;
    for (int i = 0; i < count(); i++) {
        if (testBit(i))
            occupiedList << bitToSquare(i);
    }
    return occupiedList;
}

void BitBoard::setBoard(const SquareList &squareList)
{
    if (squareList.isEmpty())
        return;

    if (!fill(false))
        return;

    foreach (Square square, squareList) {
        if (square.isValid())
            setSquare(square);
    }
}

void BitBoard::setSquare(Square square)
{
    int bit = squareToBit(square);
    setBit(bit);
}

Square BitBoard::bitToSquare(int bit) const
{
    int file = bit % 8;
    int rank = bit / 8;
    return Square(file, rank);
}

int BitBoard::squareToBit(Square square) const
{
    int f = square.file() + 1;
    int r = square.rank() + 1;
    int bit = (8 * r) - (8 - f) - 1;
    return bit;
}

QDebug operator<<(QDebug debug, const BitBoard &b) {
    debug.nospace() << "\n";
    for (int i = 7; i > -1; --i) {
        debug.nospace() << "\n";
        for (int j = 0; j < 8; ++j) {
            Square square(j, i);
            if (b.isSquareOccupied(square))
                debug.nospace() << "1";
            else
                debug.nospace() << "0";
        }
    }

    return debug.space();
}
