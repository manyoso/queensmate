#include "pgn.h"

#include <QFile>
#include <QDebug>
#include <QObject>

#include "game.h"
#include "chess.h"

using namespace Chess;

/*
1) Event (the name of the tournament or match event)

2) Site (the location of the event)

3) Date (the starting date of the game)

4) Round (the playing round ordinal of the game)

5) White (the player of the white pieces)

6) Black (the player of the black pieces)

7) Result (the result of the game)
*/

QList<Pgn> Pgn::pgnToGames(const QString &fileName, bool *ok, QString *err)
{
    QFile file(fileName);
    if (!file.exists()) {
        *ok = false;
        *err = QObject::tr("File does not exist!");
        return QList<Pgn>();
    }

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        *ok = false;
        *err = QObject::tr("Could not open file!");
        return QList<Pgn>();
    }

    QList<Pgn> games;

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        //qDebug() << "pgn line:" << line << endl;

        switch (state) {
        case TagPairSection:
        case MoveTextSection:
        default:
            {
                *ok = false;
                *err = QObject::tr("Unknown state!");
                return QList<Pgn>();
            }
        }
    }

    *ok = true;
    *err = QString();
    return games;
}

void Pgn::parseTagPair(const QByteArray &line)
{
}

void Pgn::parseMoveText(const QByteArray &line)
{
}

QString Pgn::gameToPgn(Game *game, bool *ok, QString *err)
{
    Q_UNUSED(game);
    Q_UNUSED(ok);
    Q_UNUSED(err);
    return QString();
}

Pgn::Pgn()
{
}

Pgn::~Pgn()
{
}
