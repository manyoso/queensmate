#ifndef PGN_H
#define PGN_H

#include <QHash>
#include <QString>

#include "move.h"

class Pgn {
public:
    Pgn();
    ~Pgn();

    QList<Move> moves() const { return m_moves.values(); }

    void addTag(const QString &name, const QString &value);
    void addMoveNumber(int number);
    void addMove(const Move &move);

private:
    QHash<QString, QString> m_tags;
    int m_currentMoveNumber;
    QHash<int, Move> m_moves;
    friend class PgnParser;
};

#endif
