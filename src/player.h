#ifndef PLAYER_H
#define PLAYER_H

#include <QObject>

#include "game.h"
#include "chess.h"

class Move;

class Player : public QObject {
    Q_OBJECT
public:
    Player(Game *parent);
    ~Player();

    Game *game() const { return qobject_cast<Game*>(parent()); }

    Chess::Army army() const { return m_army; }

    virtual void startGame() {}
    virtual void endGame() {}
    virtual void makeNextMove() const {}
    virtual bool isReady() const { return true; }
    virtual bool isHuman() const { return true; }
    virtual bool isRemote() const { return false; }

    QString playerName() const { return m_playerName; }
    void setPlayerName(const QString &playerName) { m_playerName = playerName; }

Q_SIGNALS:
    void ready();
    void madeMove(Move move);
    void resign();
    void offerDraw();

private:
    void setArmy(Chess::Army army) { m_army = army; }

private:
    Chess::Army m_army;
    QString m_playerName;
    friend class Game;
};

#endif
