#ifndef PGNPARSER_H
#define PGNPARSER_H

#include <QThread>

class Pgn;
class Move;
class PgnTokenStream;
typedef QList<Pgn> PgnList;

class PgnParser : public QThread {
    Q_OBJECT
public:
    PgnParser(QObject *parent);
    ~PgnParser();

    void parsePgn(const QByteArray &data);

Q_SIGNALS:
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void error(const QString &error);
    void finished(const PgnList &games);

protected:
    virtual void run();

private Q_SLOTS:
    void lexProgressOut(qint64 pos, qint64 size);
    void parseProgressOut(qint64 bytesReceived, qint64 bytesTotal);

private:
    bool parseTagPair(PgnTokenStream *stream, Pgn *pgn);
    bool parseMoveText(PgnTokenStream *stream, Pgn *pgn);
    bool parseMove(PgnTokenStream *stream, Move *move);
    bool parseResult(const QString &result);

private:
    QByteArray m_data;
};

#endif
