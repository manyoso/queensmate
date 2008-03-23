#include "pgnparser.h"

#include <QFile>
#include <QDebug>
#include <QObject>

#include "pgn.h"
#include "chess.h"
#include "pgnlexer.h"
#include "notation.h"

using namespace Chess;

QList<Pgn> PgnParser::parsePgn(const QString &fileName, bool *ok, QString *err)
{
//     qDebug() << "parsePgn" << fileName << endl;
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

    PgnLexer lexer;
    QByteArray text = file.readAll();
    PgnTokenStream stream = lexer.lex(text);

    Pgn pgn;
    while (!stream.atEnd()) {
        switch (stream.lookAhead()) {
        case PgnToken::Unknown:
            {
                *ok = false;
                *err = QString("Unknown token at '%1!'").arg(QString::number(stream.pos()));
                return QList<Pgn>();
            }
        case PgnToken::LeftBrack:
            {
                if (stream.lookAhead(1) == PgnToken::Symbol &&
                    stream.lookAhead(2) == PgnToken::String &&
                    stream.lookAhead(3) == PgnToken::RightBrack) {

                    if (!parseTagPair(&stream, &pgn, ok, err))
                        return QList<Pgn>();
                } else {
                    *ok = false;
                    *err = QString("Could not parse tag pair at '%1!'").arg(QString::number(stream.pos()));
                    return QList<Pgn>();
                }
                break;
            }
        default:
            {
                if (!parseMoveText(&stream, &pgn, ok, err))
                    return QList<Pgn>();
                games << pgn;
                pgn = Pgn();
                break;
            }
        }

        stream.next();
    }

    *ok = true;
    *err = QString();
    return games;
}

bool PgnParser::parseTagPair(PgnTokenStream *stream, Pgn *pgn, bool *ok, QString *err)
{
    Q_UNUSED(ok);
    Q_UNUSED(err);

    Q_ASSERT(stream->lookAhead() == PgnToken::LeftBrack);

    stream->next();
    QByteArray name = stream->text();
    stream->next();
    QByteArray value = stream->text();
    stream->next();

    pgn->addTag(name, value);

    return true;
}

bool PgnParser::parseMoveText(PgnTokenStream *stream, Pgn *pgn, bool *ok, QString *err)
{
    Q_UNUSED(pgn);

    while (!stream->atEnd()) {
/*        qDebug() << "token:" << stream->token() << "text:"  << stream->text() << endl;*/
        switch (stream->lookAhead()) {
        case PgnToken::Asterisk:
            break; //game termination
        case PgnToken::LeftParen:
        case PgnToken::RightParen:
            break; //recursive variation
        case PgnToken::LeftAngle:
        case PgnToken::RightAngle:
            break; //reserved for future
        case PgnToken::NAG:
            {
                break;
            }
        case PgnToken::Integer:
            {
                bool isInt = false;
                int num = stream->text().toInt(&isInt);
                Q_ASSERT(isInt);
                pgn->addMoveNumber(num);
                for (int i = 1; i < 255; ++i) {
                    if (stream->lookAhead(i) != PgnToken::Period) {
                        stream->seek(stream->pos() + i - 1);
                        break;
                    }
                }
                break;
            }
        case PgnToken::Symbol:
            {
                Move move;
                if (!parseMove(stream, &move, ok, err)) {
                    return false;
                } else {
                    pgn->addMove(move);
                }
                break;
            }
        default:
            {
                *ok = false;
                *err = QString("Unknown token in move text at '%1!'").arg(QString::number(stream->pos()));
                return false;
            }
        }
        if (stream->lookAhead(1) == PgnToken::LeftBrack)
            return true; //end of the movetext

        stream->next();
    }
    return true;
}

bool PgnParser::parseMove(PgnTokenStream *stream, Move *move, bool *ok, QString *err)
{
//    qDebug() << "pgn move" << stream->text() << endl;
    *move = Notation::stringToMove(stream->text(), Standard, ok, err);
    return ok;
}

PgnParser::PgnParser()
{
}

PgnParser::~PgnParser()
{
}
