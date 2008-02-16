#ifndef PGN_H
#define PGN_H

#include <QString>

class Game;

class Pgn {
public:
    static QList<Pgn> pgnToGames(const QString &fileName, bool *ok = 0, QString *err = 0);
    static QString gameToPgn(Game *game, bool *ok = 0, QString *err = 0);

private:
    void parseTagPair(const QByteArray &line);
    void parseMoveText(const QByteArray &line);

private:
    Pgn();
    ~Pgn();
};

#endif
