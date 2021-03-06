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
        return m_armyMoveBoards.value(army) | m_armyAttackBoards.value(army);
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

    if (move.isEnPassant()) {
        BitBoard board;
        board.setSquare(game()->enPassantTarget());
        return BitBoard(bitBoard(move.start(), Attacks) & board).isSquareOccupied(move.end());
    }

    if (move.isCastle()) {
        if (move.isKingSideCastle() && isCastleLegal(army, KingSide)) {
            if (army == White) {
                if (move.piece() == King && move.end() == Square(6, 0) /*g1*/)
                    return true;
                else if (move.piece() == Rook && move.end() == Square(5, 0) /*f1*/)
                    return true;
            } else if (army == Black) {
                if (move.piece() == King && move.end() == Square(6, 7) /*g8*/)
                    return true;
                else if (move.piece() == Rook && move.end() == Square(5, 7) /*f8*/)
                    return true;
            }
        } else if (move.isQueenSideCastle() && isCastleLegal(army, QueenSide)) {
            if (army == White) {
                if (move.piece() == King && move.end() == Square(2, 0) /*c1*/)
                    return true;
                else if (move.piece() == Rook && move.end() == Square(3, 0) /*d1*/)
                    return true;
            } else if (army == Black) {
                if (move.piece() == King && move.end() == Square(2, 7) /*c8*/)
                    return true;
                else if (move.piece() == Rook && move.end() == Square(3, 7) /*d8*/)
                    return true;
            }
        }
    }

    return false;
}

bool Rules::isChecked(Chess::Army army) const
{
    if (army == White) {
        BitBoard b(bitBoard(White) & bitBoard(King) & bitBoard(Black, Attacks));
        bool checked = !b.isClear();
/*        qDebug() << "white king is checked" << (checked ? "true" : "false")
                 << "\nwhite king" << BitBoard(bitBoard(White) & bitBoard(King))
                 << "\nblack attacks" << bitBoard(Black, Attacks) << endl;*/
        return checked;
    } else {
        BitBoard b(bitBoard(Black) & bitBoard(King) & bitBoard(White, Attacks));
        bool checked = !b.isClear();
/*        qDebug() << "black king is checked" << (checked ? "true" : "false")
                 << "\nblack king" << BitBoard(bitBoard(Black) & bitBoard(King))
                 << "\nwhite attacks" << bitBoard(White, Attacks) << endl;*/
        return checked;
    }
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

    BitBoard moves;
    if (army == White)
        moves = BitBoard(bitBoard(White, Moves) & bitBoard(White, Attacks));
    else
        moves = BitBoard(bitBoard(Black, Moves) & bitBoard(Black, Attacks));

    BitBoard attackedBy = bitBoard(square, AttackedBy);

    //King can move to escape check??
    BitBoard kingsMoves(bitBoard(army, Moves) & bitBoard(army, Attacks) &
                        bitBoard(King, Moves) & bitBoard(King, Attacks));
    bool canMove = BitBoard(kingsMoves & bitBoard(army == White ? Black : White, Attacks)) != kingsMoves;
    if (canMove) {
        qDebug() << "king can move to escape check!"
                 << "\nmoves" << kingsMoves
                 << "\nattacks" << bitBoard(army == White ? Black : White, Attacks)
                 << endl;
        return false;
    }

    //Only thing a king could do with multiple attackers is move to escape check so...
    if (attackedBy.count(true) > 1) {
        qDebug() << "multiple attackers and king can't move!" << endl;
        return true;
    }

    //A friendly piece can move to capture kings attacker??
    bool canCaptureAttacker = !BitBoard(bitBoard(army, Attacks) & attackedBy).isClear();
    if (canCaptureAttacker) {
        qDebug() << "king can capture attacker!" << endl;
        return false;
    }

    return false; //FIXME ASSERTS after this!!

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
    if (canBlock) {
        qDebug() << "can block attacker!" << endl;
        return false;
    }

    qDebug() << "that's all folks..." << endl;
    return true;
}

bool Rules::isUnderAttack(Piece piece) const
{
    return !bitBoard(piece.square(), AttackedBy).isClear();
}

bool Rules::isCastleLegal(Chess::Army army, Chess::Castle castle) const
{
    //Check if castle is available... ie, if neither king nor rook(s) have moved...
    if (!isCastleAvailable(army, castle)) {
        qDebug() << "castle is unavailable!" << endl;
        return false;
    }

    BitBoard castleBoard = bitBoard(army, castle);

    //Check if all squares between king and rook(s) are under attack...
    if (!BitBoard(castleBoard & bitBoard(army == White ? Black : White, Attacks)).isClear()) {
        qDebug() << "square is under attack!" << endl;
        return false;
    }

    //Check if all squares between king and rook(s) are unoccupied by anything other than king or rook...
    BitBoard kingBoard(bitBoard(King) & bitBoard(army));
    BitBoard rookBoard(Square(castle == KingSide ? game()->fileOfKingsRook() : game()->fileOfQueensRook(), army == White ? 0 : 7));
    BitBoard piecesBoard(kingBoard | rookBoard);
    BitBoard castleBoardMinusPieces(castleBoard ^ piecesBoard);
    if (!BitBoard(castleBoardMinusPieces & BitBoard(bitBoard(White) | bitBoard(Black))).isClear()) {
        qDebug() << "castle is impeded by occupied square!" << endl;
//         qDebug() << "castleBoard" << castleBoard << endl;
//         qDebug() << "kingBoard" << kingBoard << endl;
//         qDebug() << "rookBoard" << rookBoard << endl;
//         qDebug() << "piecesBoard" << piecesBoard << endl;
//         qDebug() << "castleBoardMinusPieces" << castleBoardMinusPieces << endl;
        return false;
    }

    return true;
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
    if (move.isCastle()) {
        SquareList kings = BitBoard(bitBoard(King) & bitBoard(army)).occupiedSquares();
        if (!kings.isEmpty())
            return kings.first();
        else
            return Square();
    }

    BitBoard positions(bitBoard(move.piece(), Positions) & bitBoard(army, Positions));
    BitBoard opposingPositions(bitBoard(army == White ? Black : White, Positions));
    for (int i = 0; i < 64; i++) {
        if (!positions.testBit(i))
            continue;

        if (!m_squareMoves.contains(i) && !m_squareAttacks.contains(i))
            continue;

        Square square = positions.bitToSquare(i);

        if (move.fileOfDeparture() != -1 && move.fileOfDeparture() != square.file())
            continue;
        if (move.rankOfDeparture() != -1 && move.rankOfDeparture() != square.rank())
            continue;

        if (m_squareMoves.value(i).isSquareOccupied(move.end())) {
            return square;
        } else if (m_squareAttacks.value(i).isSquareOccupied(move.end()) &&
                   opposingPositions.isSquareOccupied(move.end())) {
            return square;
        } else if (m_squareAttacks.value(i).isSquareOccupied(move.end()) &&
                   move.piece() == Pawn && move.end() == game()->enPassantTarget()) {
            return square;
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

void Rules::refreshCastleBoards()
{
    SquareList whiteKings = BitBoard(bitBoard(King) & bitBoard(White)).occupiedSquares();
    SquareList blackKings = BitBoard(bitBoard(King) & bitBoard(Black)).occupiedSquares();
    Square whiteKing = !whiteKings.isEmpty() ? whiteKings.first() : Square();
    Square blackKing = !blackKings.isEmpty() ? blackKings.first() : Square();
    if (!whiteKing.isValid() && !blackKing.isValid())
        return; //could be scratch board...

    int kingsRook = game()->fileOfKingsRook();
    int queensRook = game()->fileOfQueensRook();

    SquareList kc;
    SquareList qc;
    for (int i= 0; i < 8; ++i) {
        if (whiteKing.file() < kingsRook) {
            if (i >= whiteKing.file() && i <= kingsRook) {
                kc << Square(i, 0);
            }
        } else {
            if (i >= kingsRook && i <= whiteKing.file()) {
                kc << Square(i, 0);
            }
        }
        if (blackKing.file() < kingsRook) {
            if (i >= blackKing.file() && i <= kingsRook) {
                kc << Square(i, 7);
            }
        } else {
            if (i >= kingsRook && i <= blackKing.file()) {
                kc << Square(i, 7);
            }
        }
        if (whiteKing.file() < queensRook) {
            if (i >= whiteKing.file() && i <= queensRook) {
                qc << Square(i, 0);
            }
        } else {
            if (i >= queensRook && i <= whiteKing.file()) {
                qc << Square(i, 0);
            }
        }
        if (blackKing.file() < queensRook) {
            if (i >= blackKing.file() && i <= queensRook) {
                qc << Square(i, 7);
            }
        } else {
            if (i >= queensRook && i <= blackKing.file()) {
                qc << Square(i, 7);
            }
        }
    }

    m_castleBoards.insert(KingSide, BitBoard(kc));
    m_castleBoards.insert(QueenSide, BitBoard(qc));
}

void Rules::refreshBoards()
{
    refreshPositionBoards();
    refreshMoveAndAttackBoards();
    refreshCastleBoards();
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
