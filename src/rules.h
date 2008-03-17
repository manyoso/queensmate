#ifndef RULES_H
#define RULES_H

#include <QObject>

#include "game.h"

class BitBoard;

/* TODO
 * Stalemate rule...
 * Draw rule...
 */

class Rules : public QObject {
    Q_OBJECT
public:
    Rules(Game *parent);
    ~Rules();

    Game *game() const { return qobject_cast<Game*>(parent()); }

    BitBoard bitBoard(Chess::Army army, Chess::Castle castle) const;
    BitBoard bitBoard(Chess::Army army, Chess::BoardType type = Chess::Positions) const;
    BitBoard bitBoard(Chess::PieceType piece, Chess::BoardType type = Chess::Positions) const;
    BitBoard bitBoard(Square square, Chess::BoardType type = Chess::Positions) const;

    bool isLegalMove(Chess::Army army, Move move) const;
    bool isChecked(Chess::Army army) const;
    bool isCheckMated(Chess::Army army) const;
    bool isUnderAttack(Piece piece) const;

    bool isCastleLegal(Chess::Army army, Chess::Castle castle) const;
    bool isCastleAvailable(Chess::Army army, Chess::Castle castle) const;
    void setCastleAvailable(Chess::Army army, Chess::Castle castle, bool available);

    Square guessSquare(Chess::Army army, Move move) const;

private Q_SLOTS:
    void refreshPositionBoards();
    void refreshMoveAndAttackBoards();
    void refreshCastleBoards();
    void refreshBoards();

private:
    enum Direction { North, NorthEast, East, SouthEast, South, SouthWest, West, NorthWest };
    SquareList raysForKing(Piece piece);
    SquareList raysForQueen(Piece piece);
    SquareList raysForRook(Piece piece);
    SquareList raysForBishop(Piece piece);
    SquareList raysForKnight(Piece piece);
    SquareList raysForPawn(Piece piece);
    SquareList raysForPawnAttack(Piece piece);
    void generateRay(Square one, Square two, SquareList *rays) const;
    void generateRay(Direction direction, int magnitude, Piece piece, SquareList *rays, SquareList *defense) const;
    bool pieceCanMoveTo(Piece piece, Square square, bool *isCapture) const;
    bool pieceCanAttackAt(Piece piece, Square square, bool *isCapture) const;
    bool pieceCanDefendAt(Piece piece, Square square) const;

private:
    QHash<Chess::Castle, BitBoard> m_castleBoards;
    QHash<Chess::Army, BitBoard> m_armyPositionBoards;
    QHash<Chess::Army, BitBoard> m_armyMoveBoards;
    QHash<Chess::Army, BitBoard> m_armyAttackBoards;
    QHash<Chess::PieceType, BitBoard> m_pieceMoveBoards;
    QHash<Chess::PieceType, BitBoard> m_pieceAttackBoards;
    QHash<Chess::PieceType, BitBoard> m_piecePositionBoards;
    QHash<int, BitBoard> m_squareMoves;
    QHash<int, BitBoard> m_squareAttacks;
    QHash<int, BitBoard> m_squareDefends;

    bool m_hasWhiteKingCastle;
    bool m_hasBlackKingCastle;
    bool m_hasWhiteQueenCastle;
    bool m_hasBlackQueenCastle;
    friend class Game;
};

#endif
