#ifndef CHESS_H
#define CHESS_H

#include <QList>
#include <QChar>

namespace Chess {

enum BoardType
{
    Positions,
    Moves,
    Attacks,
    Defends,
    AttackedBy,
    DefendedBy
};

enum Army
{
    White,
    Black
};

enum Castle
{
    KingSide,
    QueenSide
};

enum PieceType
{
    Unknown,
    King,
    Queen,
    Rook,
    Bishop,
    Knight,
    Pawn
};

enum NotationType
{
    Standard, /* The standard algebraic notation found in PGN. */
    Long,     /* Hyphenated long algebraic notation. */
    Computer  /* Un-hyphenated long algebraic notation. UCI uses this for example. */
};

}

#endif
