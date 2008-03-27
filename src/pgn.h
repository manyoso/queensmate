#ifndef PGN_H
#define PGN_H

#include <QMap>
#include <QString>
#include <QMetaType>

#include "move.h"

class Pgn {
public:
    Pgn();
    ~Pgn();

    QString tag(const QString &name) const;
    QList<Move> moves() const { return m_moves.values(); };

    void addTag(const QString &name, const QString &value);
    void addMoveNumber(int number);
    void addMove(const Move &move);

private:
    QMap<QString, QString> m_tags;
    int m_currentMoveNumber;
    QMap<int, Move> m_moves;
    friend class PgnParser;
};

typedef QList<Pgn> PgnList;
Q_DECLARE_METATYPE(PgnList)

#endif
