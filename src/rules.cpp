#include "rules.h"

#include <QDebug>
#include <QStringList>

#include "bitboard.h"
#include "notation.h"
#include "application.h"

using namespace Chess;

Rules::Rules(Game *parent)
    : QObject(parent),
      m_hasWhiteKingCastle(false),
      m_hasBlackKingCastle(false),
      m_hasWhiteQueenCastle(false),
      m_hasBlackQueenCastle(false)
{
    QStringList kingCastle;
    kingCastle << "e1" << "f1" << "g1" << "h1";
    kingCastle << "e8" << "f8" << "g8" << "h8";
    QStringList queenCastle;
    queenCastle << "a1" << "b1" << "c1" << "d1" << "e1";
    queenCastle << "a8" << "b8" << "c8" << "d8" << "e8";

    SquareList kc;
    foreach (QString square, kingCastle) {
        kc << Notation::stringToSquare(square);
    }
    SquareList qc;
    foreach (QString square, queenCastle) {
        qc << Notation::stringToSquare(square);
    }

    m_castleBoards.insert(KingSide, BitBoard(kc));
    m_castleBoards.insert(QueenSide, BitBoard(qc));

    connect(parent, SIGNAL(pieceMoved()), this, SLOT(refreshBoards()));
}

Rules::~Rules()
{
}

BitBoard Rules::bitBoard(Chess::Army army, Chess::Castle castle) const
{
    return m_castleBoards.value(castle) & m_armyPositionBoards.value(army);
}

BitBoard Rules::bitBoard(Chess::Army army, Chess::BoardType type) const
{
    switch (type) {
    case Positions:
        return m_armyPositionBoards.value(army);
    case Moves:
        return m_armyMoveBoards.value(army);
    case Attacks:
        return m_armyAttackBoards.value(army);
    case Defends:
        return BitBoard();
    case DefendedBy:
        return BitBoard();
    case AttackedBy:
        return BitBoard();
    default:
        break;
    }
    return BitBoard();
}

BitBoard Rules::bitBoard(Chess::PieceType piece, Chess::BoardType type) const
{
    switch (type) {
    case Positions:
        return m_piecePositionBoards.value(piece);
    case Moves:
        return m_pieceMoveBoards.value(piece);
    case Attacks:
        return m_pieceAttackBoards.value(piece);
    case Defends:
        return BitBoard();
    case DefendedBy:
        return BitBoard();
    case AttackedBy:
        return BitBoard();
    default:
        break;
    }
    return BitBoard();
}

BitBoard Rules::bitBoard(Square square, Chess::BoardType type) const
{
    switch (type) {
    case Positions:
        {
            BitBoard b;
            b.setBit(square.index(), true);
            return b;
        }
    case Moves:
        return m_squareMoves.value(square.index());
    case Attacks:
        return m_squareAttacks.value(square.index());
    case Defends:
        return m_squareDefends.value(square.index());
    case DefendedBy:
        {
            BitBoard b;
            foreach (int index, m_squareDefends.keys()) {
                if (m_squareDefends.value(index).isSquareOccupied(square))
                    b.setBit(index, true);
            }
            return b;
        }
    case AttackedBy:
        {
            BitBoard b;
            foreach (int index, m_squareAttacks.keys()) {
                if (m_squareAttacks.value(index).isSquareOccupied(square))
                    b.setBit(index, true);
            }
            return b;
        }
    default:
        break;
    }
    return BitBoard();
}

bool Rules::isLegalMove(Chess::Army army, Move move) const
{
    bool moves = bitBoard(move.start(), Moves).isSquareOccupied(move.end());
    if (moves) {
        return true;
    }

    bool attacks = BitBoard(bitBoard(move.start(), Attacks) & bitBoard(army == White ? Black : White)).isSquareOccupied(move.end());
    if (attacks) {
        return true;
    }

    if (move.isCastle()) {
        if (army == White && move.isKingSideCastle()) {
            if (isCastleAvailable(army, KingSide) && move.end() == Square(6, 0) /*g1*/)
                return true;
        } else if (army == White && move.isQueenSideCastle()) {
            if (isCastleAvailable(army, QueenSide) && move.end() == Square(2, 0) /*c1*/)
                return true;
        } else if (army == Black && move.isKingSideCastle()) {
            if (isCastleAvailable(army, KingSide) && move.end() == Square(6, 7) /*g8*/)
                return true;
        } else if (army == Black && move.isQueenSideCastle()) {
            if (isCastleAvailable(army, QueenSide) && move.end() == Square(2, 7) /*c8*/)
                return true;
        }
    }

    return false;
}

bool Rules::isChecked(Chess::Army army) const
{
    if (army == White)
        return !BitBoard(bitBoard(White) & bitBoard(King) & bitBoard(Black, Attacks)).isClear();
    else
        return !BitBoard(bitBoard(Black) & bitBoard(King) & bitBoard(White, Attacks)).isClear();
    return false;
}

bool Rules::isCheckMated(Chess::Army army) const
{
    if (!isChecked(army))
        return false;

    Square square;
    if (army == White)
        square = BitBoard(bitBoard(White) & bitBoard(King)).occupiedSquares().first();
    else
        square = BitBoard(bitBoard(Black) & bitBoard(King)).occupiedSquares().first();

    BitBoard kingsMoves;
    if (army == White)
        kingsMoves = BitBoard(bitBoard(White, Moves) & bitBoard(White, Attacks));
    else
        kingsMoves = BitBoard(bitBoard(Black, Moves) & bitBoard(Black, Attacks));

    BitBoard attackedBy = bitBoard(square, AttackedBy);

    //King can move to escape check??
    bool canMove = BitBoard(kingsMoves & bitBoard(army == White ? Black : White, Attacks)) != kingsMoves;
    if (canMove)
        return false;

    //Only thing a king could do with multiple attackers is move to escape check so...
    if (attackedBy.count(true) > 1)
        return true;

    //A friendly piece can move to capture kings attacker??
    bool canCaptureAttacker = !BitBoard(bitBoard(army, Attacks) & attackedBy).isClear();
    if (canCaptureAttacker)
        return false;

    //Another piece can move to block attacker??
    Square attacker;
    for (int i = 0; i < 64; i++) {
        if (!attackedBy.testBit(i))
            continue;

        attacker = attackedBy.bitToSquare(i); break;
    }
    Q_ASSERT(attacker.isValid());

    SquareList squareList;
    generateRay(attacker, square, &squareList);
    BitBoard rayOfAttack(squareList);

    bool canBlock = BitBoard(bitBoard(army, Moves) & rayOfAttack) != BitBoard(bitBoard(army, Moves));
    if (canBlock)
        return true;

    return false;
}

bool Rules::isUnderAttack(Piece piece) const
{
    return !bitBoard(piece.square(), AttackedBy).isClear();
}

bool Rules::isCastleAvailable(Chess::Army army, Chess::Castle castle) const
{
    if (army == White && castle == KingSide) {
        if (!m_hasWhiteKingCastle) {
            return false;
        }
    } else if (army == Black && castle == KingSide) {
        if (!m_hasBlackKingCastle) {
            return false;
        }
    } else if (army == White && castle == QueenSide) {
        if (!m_hasWhiteQueenCastle) {
            return false;
        }
    } else if (army == Black && castle == QueenSide) {
        if (!m_hasBlackQueenCastle) {
            return false;
        }
    }

    return true;
}

void Rules::setCastleAvailable(Chess::Army army, Chess::Castle castle, bool available)
{
    if (army == White && castle == KingSide) {
        m_hasWhiteKingCastle = available;
    } else if (army == Black && castle == KingSide) {
        m_hasBlackKingCastle = available;
    } else if (army == White && castle == QueenSide) {
        m_hasWhiteQueenCastle = available;
    } else if (army == Black && castle == QueenSide) {
        m_hasBlackQueenCastle = available;
    }
}

Square Rules::guessSquare(Chess::Army army, Move move) const
{
    BitBoard b(bitBoard(move.piece(), Positions) & bitBoard(army, Positions));
    for (int i = 0; i < 64; i++) {
        if (!b.testBit(i))
            continue;

        if (!m_squareMoves.contains(i))
            continue;

        if (m_squareMoves.value(i).isSquareOccupied(move.end())) {
            return b.bitToSquare(i);
        }
    }
    return Square();
}

void Rules::refreshPositionBoards()
{
    SquareList kingSquares;
    SquareList queenSquares;
    SquareList rookSquares;
    SquareList bishopSquares;
    SquareList knightSquares;
    SquareList pawnSquares;

    SquareList whiteSquares;

    {
    PieceList whitePieces= game()->pieces(White);
    PieceList::ConstIterator it = whitePieces.begin();
    for (; it != whitePieces.end(); ++it) {
        whiteSquares << (*it).square();

        switch ((*it).piece()) {
        case King:
            kingSquares << (*it).square();
            break;
        case Queen:
            queenSquares << (*it).square();
            break;
        case Rook:
            rookSquares << (*it).square();
            break;
        case Bishop:
            bishopSquares << (*it).square();
            break;
        case Knight:
            knightSquares << (*it).square();
            break;
        case Pawn:
            pawnSquares << (*it).square();
            break;
        case Unknown:
        default:
            break;
        }
    }
    }

    m_armyPositionBoards.insert(White, BitBoard(whiteSquares));

    SquareList blackSquares;

    {
    PieceList blackPieces = game()->pieces(Black);
    PieceList::ConstIterator it = blackPieces.begin();
    for (; it != blackPieces.end(); ++it) {
        blackSquares << (*it).square();

        switch ((*it).piece()) {
        case King:
            kingSquares << (*it).square();
            break;
        case Queen:
            queenSquares << (*it).square();
            break;
        case Rook:
            rookSquares << (*it).square();
            break;
        case Bishop:
            bishopSquares << (*it).square();
            break;
        case Knight:
            knightSquares << (*it).square();
            break;
        case Pawn:
            pawnSquares << (*it).square();
            break;
        case Unknown:
        default:
            break;
        }
    }
    }

    m_armyPositionBoards.insert(Black, BitBoard(blackSquares));

    m_piecePositionBoards.insert(King, BitBoard(kingSquares));
    m_piecePositionBoards.insert(Queen, BitBoard(queenSquares));
    m_piecePositionBoards.insert(Rook, BitBoard(rookSquares));
    m_piecePositionBoards.insert(Bishop, BitBoard(bishopSquares));
    m_piecePositionBoards.insert(Knight, BitBoard(knightSquares));
    m_piecePositionBoards.insert(Pawn, BitBoard(pawnSquares));
}

void Rules::refreshMoveAndAttackBoards()
{
    m_squareMoves.clear();
    m_squareAttacks.clear();
    m_squareDefends.clear();

    SquareList whiteKingSquares;
    SquareList whiteQueenSquares;
    SquareList whiteRookSquares;
    SquareList whiteBishopSquares;
    SquareList whiteKnightSquares;
    SquareList whitePawnSquares;
    SquareList whitePawnAttackSquares;

    {
    PieceList whitePieces= game()->pieces(White);
    PieceList::ConstIterator it = whitePieces.begin();
    for (; it != whitePieces.end(); ++it) {
        switch ((*it).piece()) {
        case King:
            {
                SquareList rays = raysForKing(*it);
                whiteKingSquares << rays;
                break;
            }
        case Queen:
            {
                SquareList rays = raysForQueen(*it);
                whiteQueenSquares << rays;
                break;
            }
        case Rook:
            {
                SquareList rays = raysForRook(*it);
                whiteRookSquares << rays;
                break;
            }
        case Bishop:
            {
                SquareList rays = raysForBishop(*it);
                whiteBishopSquares << rays;
                break;
            }
        case Knight:
            {
                SquareList rays = raysForKnight(*it);
                whiteKnightSquares << rays;
                break;
            }
        case Pawn:
            whitePawnSquares << raysForPawn(*it);
            whitePawnAttackSquares << raysForPawnAttack(*it);
            break;
        case Unknown:
        default:
            break;
        }
    }
    }

    SquareList whiteMoveSquares;
    whiteMoveSquares << whiteKingSquares << whiteQueenSquares << whiteRookSquares
                     << whiteBishopSquares << whiteKnightSquares << whitePawnSquares;

    m_armyMoveBoards.insert(White, BitBoard(whiteMoveSquares));
    m_armyAttackBoards.insert(White, BitBoard(whitePawnAttackSquares));

    SquareList blackKingSquares;
    SquareList blackQueenSquares;
    SquareList blackRookSquares;
    SquareList blackBishopSquares;
    SquareList blackKnightSquares;
    SquareList blackPawnSquares;
    SquareList blackPawnAttackSquares;

    {
    PieceList blackPieces= game()->pieces(Black);
    PieceList::ConstIterator it = blackPieces.begin();
    for (; it != blackPieces.end(); ++it) {
        switch ((*it).piece()) {
        case King:
            {
                SquareList rays = raysForKing(*it);
                blackKingSquares << rays;
                break;
            }
        case Queen:
            {
                SquareList rays = raysForQueen(*it);
                blackQueenSquares << rays;
                break;
            }
        case Rook:
            {
                SquareList rays = raysForRook(*it);
                blackRookSquares << rays;
                break;
            }
        case Bishop:
            {
                SquareList rays = raysForBishop(*it);
                blackBishopSquares << rays;
                break;
            }
        case Knight:
            {
                SquareList rays = raysForKnight(*it);
                blackKnightSquares << rays;
                break;
            }
        case Pawn:
            blackPawnSquares << raysForPawn(*it);
            blackPawnAttackSquares << raysForPawnAttack(*it);
            break;
        case Unknown:
        default:
            break;
        }
    }
    }

    SquareList blackMoveSquares;
    blackMoveSquares << blackKingSquares << blackQueenSquares << blackRookSquares
                     << blackBishopSquares << blackKnightSquares << blackPawnSquares;

    m_armyMoveBoards.insert(Black, BitBoard(blackMoveSquares));
    m_armyAttackBoards.insert(Black, BitBoard(blackPawnAttackSquares));

    m_pieceMoveBoards.insert(King, BitBoard(whiteKingSquares << blackKingSquares));
    m_pieceMoveBoards.insert(Queen, BitBoard(whiteQueenSquares << blackQueenSquares));
    m_pieceMoveBoards.insert(Rook, BitBoard(whiteRookSquares << blackRookSquares));
    m_pieceMoveBoards.insert(Bishop, BitBoard(whiteBishopSquares << blackBishopSquares));
    m_pieceMoveBoards.insert(Knight, BitBoard(whiteKnightSquares << blackKnightSquares));
    m_pieceMoveBoards.insert(Pawn, BitBoard(whitePawnSquares << blackPawnSquares));
    m_pieceAttackBoards.insert(Pawn, BitBoard(whitePawnAttackSquares << blackPawnAttackSquares));
}

void Rules::refreshBoards()
{
    refreshPositionBoards();
    refreshMoveAndAttackBoards();
}

SquareList Rules::raysForKing(Piece piece)
{
    SquareList rays;
    SquareList defense;

    generateRay(North, 1, piece, &rays, &defense);
    generateRay(NorthEast, 1, piece, &rays, &defense);
    generateRay(East, 1, piece, &rays, &defense);
    generateRay(SouthEast, 1, piece, &rays, &defense);
    generateRay(South, 1, piece, &rays, &defense);
    generateRay(SouthWest, 1, piece, &rays, &defense);
    generateRay(West, 1, piece, &rays, &defense);
    generateRay(NorthWest, 1, piece, &rays, &defense);

    m_squareDefends.insert(piece.square().index(), BitBoard(defense));
    m_squareMoves.insert(piece.square().index(), BitBoard(rays));
    m_squareAttacks.insert(piece.square().index(), BitBoard(rays));
    return rays;
}

SquareList Rules::raysForQueen(Piece piece)
{
    SquareList rays;
    SquareList defense;

    generateRay(North, 7, piece, &rays, &defense);
    generateRay(NorthEast, 7, piece, &rays, &defense);
    generateRay(East, 7, piece, &rays, &defense);
    generateRay(SouthEast, 7, piece, &rays, &defense);
    generateRay(South, 7, piece, &rays, &defense);
    generateRay(SouthWest, 7, piece, &rays, &defense);
    generateRay(West, 7, piece, &rays, &defense);
    generateRay(NorthWest, 7, piece, &rays, &defense);

    m_squareDefends.insert(piece.square().index(), BitBoard(defense));
    m_squareMoves.insert(piece.square().index(), BitBoard(rays));
    m_squareAttacks.insert(piece.square().index(), BitBoard(rays));
    return rays;
}

SquareList Rules::raysForRook(Piece piece)
{
    SquareList rays;
    SquareList defense;

    generateRay(North, 7, piece, &rays, &defense);
    generateRay(East, 7, piece, &rays, &defense);
    generateRay(South, 7, piece, &rays, &defense);
    generateRay(West, 7, piece, &rays, &defense);

    m_squareDefends.insert(piece.square().index(), BitBoard(defense));
    m_squareMoves.insert(piece.square().index(), BitBoard(rays));
    m_squareAttacks.insert(piece.square().index(), BitBoard(rays));
    return rays;
}

SquareList Rules::raysForBishop(Piece piece)
{
    SquareList rays;
    SquareList defense;

    generateRay(NorthEast, 7, piece, &rays, &defense);
    generateRay(SouthEast, 7, piece, &rays, &defense);
    generateRay(SouthWest, 7, piece, &rays, &defense);
    generateRay(NorthWest, 7, piece, &rays, &defense);

    m_squareDefends.insert(piece.square().index(), BitBoard(defense));
    m_squareMoves.insert(piece.square().index(), BitBoard(rays));
    m_squareAttacks.insert(piece.square().index(), BitBoard(rays));
    return rays;
}

SquareList Rules::raysForKnight(Piece piece)
{
    SquareList rays;
    SquareList defense;

    Square originalSquare = piece.square();

    int f = piece.square().file();
    int r = piece.square().rank();

    //2 north then 1 east
    if (r + 2 < 8) {
        piece.setSquare(Square(f, r + 2));
        generateRay(East, 1, piece, &rays, &defense);
    }

    //2 east then 1 north
    if (f + 2 < 8) {
        piece.setSquare(Square(f + 2, r));
        generateRay(North, 1, piece, &rays, &defense);
    }

    //2 east then 1 south
    if (f + 2 < 8) {
        piece.setSquare(Square(f + 2, r));
        generateRay(South, 1, piece, &rays, &defense);
    }
    //2 south then 1 east
    if (r - 2 >= 0) {
        piece.setSquare(Square(f, r - 2));
        generateRay(East, 1, piece, &rays, &defense);
    }

    //2 south then 1 west
    if (r - 2 >= 0) {
        piece.setSquare(Square(f, r - 2));
        generateRay(West, 1, piece, &rays, &defense);
    }

    //2 west then 1 south
    if (f - 2 >= 0) {
        piece.setSquare(Square(f - 2, r));
        generateRay(South, 1, piece, &rays, &defense);
    }

    //2 west then 1 north
    if (f - 2 >= 0) {
        piece.setSquare(Square(f - 2, r));
        generateRay(North, 1, piece, &rays, &defense);
    }

    //2 north then 1 west
    if (r + 2 < 8) {
        piece.setSquare(Square(f, r + 2));
        generateRay(West, 1, piece, &rays, &defense);
    }

    m_squareDefends.insert(originalSquare.index(), BitBoard(defense));
    m_squareMoves.insert(originalSquare.index(), BitBoard(rays));
    m_squareAttacks.insert(originalSquare.index(), BitBoard(rays));
    return rays;
}

SquareList Rules::raysForPawn(Piece piece)
{
    SquareList rays;
    SquareList defense;

    switch (piece.army()) {
    case Chess::White:
        {
            if (piece.square().rank() == 1) {
                generateRay(North, 2, piece, &rays, &defense);
            } else {
                generateRay(North, 1, piece, &rays, &defense);
            }
            break;
        }
    case Chess::Black:
        {
            if (piece.square().rank() == 6) {
                generateRay(South, 2, piece, &rays, &defense);
            } else {
                generateRay(South, 1, piece, &rays, &defense);
            }
            break;
        }
    default:
        break;
    }

    m_squareMoves.insert(piece.square().index(), BitBoard(rays));
    return rays;
}

SquareList Rules::raysForPawnAttack(Piece piece)
{
    SquareList rays;
    SquareList defense;

    switch (piece.army()) {
    case Chess::White:
        {
            generateRay(NorthEast, 1, piece, &rays, &defense);
            generateRay(NorthWest, 1, piece, &rays, &defense);
            break;
        }
    case Chess::Black:
        {
            generateRay(SouthEast, 1, piece, &rays, &defense);
            generateRay(SouthWest, 1, piece, &rays, &defense);
            break;
        }
    default:
        break;
    }

    m_squareDefends.insert(piece.square().index(), BitBoard(defense));
    m_squareAttacks.insert(piece.square().index(), BitBoard(rays));
    return rays;
}

void Rules::generateRay(Square one, Square two, SquareList *rays) const
{
    bool rayIsFile = (one.file() == two.file());
    bool rayIsRank = (one.rank() == two.rank());
    bool rayIsDiagonal = (qAbs(one.file() - two.file()) == qAbs(one.rank() - two.rank()));

    if (!rayIsFile && !rayIsRank && !rayIsDiagonal)
        return;

    bool north = one.rank() < two.rank();
    bool east = one.file() < two.file();

    if (rayIsFile) {
        int count = qAbs(one.rank() - two.rank()) - 1;
        for (int i = 0; i < count; i++) {
            *rays << Square(one.file(), north ? one.rank() + i + 1 : one.rank() - i + 1);
        }
        return;
    }
    if (rayIsRank) {
        int count = qAbs(one.file() - two.file()) - 1;
        for (int i = 0; i < count; i++) {
            *rays << Square(east ? one.file() + i + 1 : one.file() - i + 1, one.rank());
        }
        return;
    }

    int count = qAbs(one.file() - two.file()) - 1; //same as rank for diagonal...
    for (int i = 0; i < count; i++) {
        *rays << Square(east ? one.file() + i + 1 : one.file() - i + 1,
                        north ? one.rank() + i + 1 : one.rank() - i + 1);
    }
}

void Rules::generateRay(Direction direction, int magnitude, Piece piece, SquareList *rays, SquareList *defense) const
{
    int initialFile = piece.square().file();
    int initialRank = piece.square().rank();

    switch (direction) {
    case North:
        {
            int count = 0;
            int r = initialRank + 1;
            for (; r < 8; r++) {
                count++;
                Square sq(initialFile, r);
                bool isCapture = false;
                bool isMoveLegal = pieceCanMoveTo(piece, sq, &isCapture);
                bool isDefense = piece.piece() != Pawn ? pieceCanDefendAt(piece, sq) : false;
                bool isOutOfBounds = count == magnitude;
                if (isMoveLegal) {
                    *rays << sq;
                }
                if (isDefense) {
                    *defense << sq;
                }
                if (!isMoveLegal || isCapture || isOutOfBounds)
                    break;
            }

            break;
        }
    case NorthEast:
        {
            int count = 0;
            int f = initialFile + 1;
            int r = initialRank + 1;
            for (; f < 8 && r < 8; f++, r++) {
                count++;
                Square sq(f, r);
                bool isCapture = false;
                bool isMoveLegal = pieceCanAttackAt(piece, sq, &isCapture);
                bool isDefense = pieceCanDefendAt(piece, sq);
                bool isOutOfBounds = count == magnitude;
                if (isMoveLegal) {
                    *rays << sq;
                }
                if (isDefense) {
                    *defense << sq;
                }
                if (!isMoveLegal || isCapture || isOutOfBounds)
                    break;
            }

            break;
        }
    case East:
        {
            int count = 0;
            int f = initialFile + 1;
            for (; f < 8; f++) {
                count++;
                Square sq(f, initialRank);
                bool isCapture = false;
                bool isMoveLegal = pieceCanMoveTo(piece, sq, &isCapture);
                bool isDefense = piece.piece() != Pawn ? pieceCanDefendAt(piece, sq) : false;
                bool isOutOfBounds = count == magnitude;
                if (isMoveLegal) {
                    *rays << sq;
                }
                if (isDefense) {
                    *defense << sq;
                }
                if (!isMoveLegal || isCapture || isOutOfBounds)
                    break;
            }

            break;
        }
    case SouthEast:
        {
            int count = 0;
            int f = initialFile + 1;
            int r = initialRank - 1;
            for (; f < 8 && r >= 0; f++, r--) {
                count++;
                Square sq(f, r);
                bool isCapture = false;
                bool isMoveLegal = pieceCanAttackAt(piece, sq, &isCapture);
                bool isDefense = pieceCanDefendAt(piece, sq);
                bool isOutOfBounds = count == magnitude;
                if (isMoveLegal) {
                    *rays << sq;
                }
                if (isDefense) {
                    *defense << sq;
                }
                if (!isMoveLegal || isCapture || isOutOfBounds)
                    break;
            }

            break;
        }
    case South:
        {
            int count = 0;
            int r = initialRank - 1;
            for (; r >= 0; r--) {
                count++;
                Square sq(initialFile, r);
                bool isCapture = false;
                bool isMoveLegal = pieceCanMoveTo(piece, sq, &isCapture);
                bool isDefense = piece.piece() != Pawn ? pieceCanDefendAt(piece, sq) : false;
                bool isOutOfBounds = count == magnitude;
                if (isMoveLegal) {
                    *rays << sq;
                }
                if (isDefense) {
                    *defense << sq;
                }
                if (!isMoveLegal || isCapture || isOutOfBounds)
                    break;
            }

            break;
        }
    case SouthWest:
        {
            int count = 0;
            int f = initialFile - 1;
            int r = initialRank - 1;
            for (; f >= 0 && r >= 0; f--, r--) {
                count++;
                Square sq(f, r);
                bool isCapture = false;
                bool isMoveLegal = pieceCanAttackAt(piece, sq, &isCapture);
                bool isDefense = pieceCanDefendAt(piece, sq);
                bool isOutOfBounds = count == magnitude;
                if (isMoveLegal) {
                    *rays << sq;
                }
                if (isDefense) {
                    *defense << sq;
                }
                if (!isMoveLegal || isCapture || isOutOfBounds)
                    break;
            }

            break;
        }
    case West:
        {
            int count = 0;
            int f = initialFile - 1;
            for (; f >= 0; f--) {
                count++;
                Square sq(f, initialRank);
                bool isCapture = false;
                bool isMoveLegal = pieceCanMoveTo(piece, sq, &isCapture);
                bool isDefense = piece.piece() != Pawn ? pieceCanDefendAt(piece, sq) : false;
                bool isOutOfBounds = count == magnitude;
                if (isMoveLegal) {
                    *rays << sq;
                }
                if (isDefense) {
                    *defense << sq;
                }
                if (!isMoveLegal || isCapture || isOutOfBounds)
                    break;
            }
            break;
        }
    case NorthWest:
        {
            int count = 0;
            int f = initialFile - 1;
            int r = initialRank + 1;
            for (; f >= 0 && r < 8; f--, r++) {
                count++;
                Square sq(f, r);
                bool isCapture = false;
                bool isMoveLegal = pieceCanAttackAt(piece, sq, &isCapture);
                bool isDefense = pieceCanDefendAt(piece, sq);
                bool isOutOfBounds = count == magnitude;
                if (isMoveLegal) {
                    *rays << sq;
                }
                if (isDefense) {
                    *defense << sq;
                }
                if (!isMoveLegal || isCapture || isOutOfBounds)
                    break;
            }

            break;
        }
    default:
        break;
    }
}

bool Rules::pieceCanMoveTo(Piece piece, Square square, bool *isCapture) const
{
    if (piece.piece() == Pawn) {
        *isCapture = false;
        return (!bitBoard(White).isSquareOccupied(square) && !bitBoard(Black).isSquareOccupied(square));
    } else {
        return pieceCanAttackAt(piece, square, isCapture);
    }
}

bool Rules::pieceCanAttackAt(Piece piece, Square square, bool *isCapture) const
{
    *isCapture = (piece.army() == White ? bitBoard(Black).isSquareOccupied(square) : bitBoard(White).isSquareOccupied(square));
    return (piece.army() == White ? !bitBoard(White).isSquareOccupied(square) : !bitBoard(Black).isSquareOccupied(square));
}

bool Rules::pieceCanDefendAt(Piece piece, Square square) const
{
    return !bitBoard(piece.army() == White ? Black : White).isSquareOccupied(square);
}
