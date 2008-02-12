#ifndef GAME_H
#define GAME_H

#include <QObject>

#include <QList>
#include <QMap>
#include <QHash>
#include <QPointer>

#include "chess.h"
#include "move.h"
#include "piece.h"
#include "square.h"

class Rules;
class Clock;
class Player;
class MovesModel;

/*Uses Forsyth-Edwards Notation*/
class Game : public QObject {
    Q_OBJECT
public:
    Game(QObject *parent);
    Game(QObject *parent, const QString &fen);
    ~Game();

    int count();
    int position() const;
    void setPosition(int index);

    QString fen(int index) const;

    Chess::Army activeArmy() const { return m_activeArmy; }

    bool isChess960() const { return m_isChess960; }
    void setChess960(bool isChess960) { m_isChess960 = isChess960; }

    bool isScratchGame() const { return m_isScratchGame; }
    void setScratchGame(bool isScratchGame) { m_isScratchGame = isScratchGame; }

    PieceList pieces(Chess::Army army) const;
    PieceList capturedPieces(Chess::Army army) const;

    int halfMoveClock() const { return m_halfMoveClock; }
    void setHalfMoveClock(int halfMoveClock) { m_halfMoveClock = halfMoveClock; }

    int fullMoveNumber() const { return m_fullMoveNumber; }
    void setFullMoveNumber(int fullMoveNumber) { m_fullMoveNumber = fullMoveNumber; }

    Square enPassantTarget() const { return m_enPassantTarget; }
    void setEnPassantTarget(Square enPassantTarget) { m_enPassantTarget = enPassantTarget; }

    Player *player(Chess::Army army) const;
    void setPlayers(Player *white, Player *black);

    bool startGame();
    bool endGame();

    bool localHumanMadeMove(Chess::Army army, Move move);
    bool remoteOrEngineMadeMove(Chess::Army army, Move move);

    Rules *rules() const { return m_rules; }
    Clock *clock() const { return m_clock; }
    MovesModel *moves() const { return m_moves; }

Q_SIGNALS:
    void pieceMoved();
    void piecesChanged();
    void capturedPiecesChanged();
    void gameStarted();
    void gameEnded();

private Q_SLOTS:
    void playerReady();
    void playerMadeMove(Move move);

private:
    void processMove(Chess::Army army, Move move);
    bool fillOutMove(Chess::Army army, Move *move);
    bool fillOutStart(Chess::Army army, Move *move);

    void setFen(const QString &fen);
    QString stateOfGameToFen() const; /* generates the fen for our current state */

private:
    int m_index;
    Chess::Army m_activeArmy;
    bool m_isChess960;
    bool m_isScratchGame;
    int m_halfMoveClock;
    int m_fullMoveNumber;
    int m_fileOfKingsRook;
    int m_fileOfQueensRook;
    Square m_enPassantTarget;
    QHash<int, Piece> m_whitePieces;    //current white pieces
    QHash<int, Piece> m_blackPieces;    //current black pieces
    PieceList m_whiteCapturedPieces;    //white pieces that have been captured
    PieceList m_blackCapturedPieces;    //black pieces that have been captured
    MoveList m_whiteHistory;            //white history of moves
    MoveList m_blackHistory;            //black history of moves
    QMap<int, QString> m_mapOfFen;      //map of fen throughout game...
    QPointer<Player> m_white;
    QPointer<Player> m_black;
    Rules *m_rules;
    Clock *m_clock;
    MovesModel *m_moves;
    friend class Board;
};

#endif
