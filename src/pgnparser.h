#ifndef PGNPARSER_H
#define PGNPARSER_H

#include <QString>

class Pgn;
class Move;
class PgnTokenStream;

class PgnParser {
public:
    static QList<Pgn> parsePgn(const QString &fileName, bool *ok = 0, QString *err = 0);

private:
    static bool parseTagPair(PgnTokenStream *stream, Pgn *pgn, bool *ok = 0, QString *err = 0);
    static bool parseMoveText(PgnTokenStream *stream, Pgn *pgn, bool *ok = 0, QString *err = 0);
    static bool parseMove(PgnTokenStream *stream, Move *move, bool *ok = 0, QString *err = 0);
    PgnParser();
    ~PgnParser();
};

#endif
