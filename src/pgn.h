#ifndef PGN_H
#define PGN_H

#include <QMap>
#include <QString>
#include <QMetaType>

#include "move.h"
#include "game.h"

class Pgn {
public:
    Pgn();
    ~Pgn();

    QString tag(const QString &name) const;
    QList<Move> moves() const { return m_moves.values(); };
    Game::Result result() const { return m_result; }

    void addTag(const QString &name, const QString &value);
    void addMoveNumber(int number);
    void addMove(const Move &move);
    void addResult(Game::Result result) { m_result = result; }

private:
    QMap<QString, QString> m_tags;
    int m_currentMoveNumber;
    QMap<int, Move> m_moves;
    Game::Result m_result;
    friend class PgnParser;
};

typedef QList<Pgn> PgnList;
Q_DECLARE_METATYPE(PgnList)

#endif
