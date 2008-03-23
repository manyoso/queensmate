#include "pgnparser.h"

#include <QFile>
#include <QDebug>
#include <QObject>

#include "pgn.h"
#include "chess.h"
#include "pgnlexer.h"

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

//     qDebug() << "pgn tag pair"
//              << "\nname" << name
//              << "\nvalue" << value
//              << endl;

    if (name == "Event")
        pgn->m_event = value;
    else if (name == "Site")
        pgn->m_site = value;
    else if (name == "Date")
        pgn->m_date = value;
    else if (name == "Round")
        pgn->m_round = value;
    else if (name == "White")
        pgn->m_white = value;
    else if (name == "Black")
        pgn->m_black = value;
    else if (name == "Result")
        pgn->m_result = value;

    pgn->m_tags.insert(name, value);

    stream->next();
    return true;
}

bool PgnParser::parseMoveText(PgnTokenStream *stream, Pgn *pgn, bool *ok, QString *err)
{
//    qDebug() << "pgn move text" << endl;
    Q_UNUSED(pgn);
    while (!stream->atEnd()) {
        switch (stream->lookAhead()) {
        case PgnToken::String:
        case PgnToken::Period:
        case PgnToken::Asterisk:
        case PgnToken::LeftParen:
        case PgnToken::RightParen:
        case PgnToken::LeftAngle:
        case PgnToken::RightAngle:
        case PgnToken::NAG:
        case PgnToken::Integer:
        case PgnToken::Symbol:
            {
                break;
            }
        default:
            {
                *ok = false;
                *err = QString("Unknown token in move text at '%1!'").arg(QString::number(stream->pos()));
                qDebug() << stream->token().toString() << endl;
                return false;
            }
        }
        if (stream->lookAhead(1) == PgnToken::LeftBrack)
            return true; //end of the movetext

        stream->next();
    }
    return true;
}

PgnParser::PgnParser()
{
}

PgnParser::~PgnParser()
{
}
