#include "pgnparser.h"

#include <QUrl>
#include <QFile>
#include <QDebug>

#include "pgn.h"
#include "chess.h"
#include "pgnlexer.h"
#include "notation.h"

using namespace Chess;

PgnParser::PgnParser(QObject *parent)
    : QThread(parent)
{
}

PgnParser::~PgnParser()
{
}

void PgnParser::parsePgn(const QByteArray &data)
{
    m_data = data;
    start(); //woohoo!
}

void PgnParser::run()
{
    qDebug() << "parsing pgn..." << endl;
    QList<Pgn> games;

    PgnLexer lexer;
    //connect(&lexer, SIGNAL(progress(qint64, qint64)), this, SLOT(lexProgressOut(qint64, qint64)));
    //connect(this, SIGNAL(progress(qint64, qint64)), this, SLOT(parseProgressOut(qint64, qint64)));
    PgnTokenStream stream = lexer.lex(m_data);

    qDebug() << "lexing finished..." << endl;

    Pgn pgn;
    while (!stream.atEnd()) {
        switch (stream.lookAhead()) {
        case PgnToken::Unknown:
            {
                emit error(QString("Unknown token at '%1!'").arg(QString::number(stream.pos())));
                return;
            }
        case PgnToken::LeftBrack:
            {
                if (stream.lookAhead(1) == PgnToken::Symbol &&
                    stream.lookAhead(2) == PgnToken::String &&
                    stream.lookAhead(3) == PgnToken::RightBrack) {

                    if (!parseTagPair(&stream, &pgn)) {
                        return;
                    }
                } else {
                    emit error(QString("Could not parse tag pair at '%1!'").arg(QString::number(stream.pos())));
                    return;
                }
                break;
            }
        default:
            {
                if (!parseMoveText(&stream, &pgn)) {
                    return;
                }
                games << pgn;
                pgn = Pgn();
                break;
            }
        }

        stream.next();
    }

    emit finished(games);
    return;
}

bool PgnParser::parseTagPair(PgnTokenStream *stream, Pgn *pgn)
{
//    qDebug() << "token:" << stream->token() << "text:"  << stream->text() << endl;
    Q_ASSERT(stream->lookAhead() == PgnToken::LeftBrack);

    stream->next();
    QByteArray name = stream->text();
    stream->next();
    QByteArray value = stream->text();
    stream->next();

    pgn->addTag(name, value);

    return true;
}

bool PgnParser::parseMoveText(PgnTokenStream *stream, Pgn *pgn)
{
    Q_UNUSED(pgn);

    while (!stream->atEnd()) {
//        qDebug() << "token:" << stream->token() << "text:"  << stream->text() << endl;
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
                QString text = stream->text();
                if (text.startsWith('0') || text.startsWith('1')) {
                    pgn->addResult(parseResult(stream->text()));
                } else {
                    Move move;
                    if (!parseMove(stream, &move)) {
                        return false;
                    } else {
                        pgn->addMove(move);
                    }
                }
                break;
            }
        default:
            {
                emit error(QString("Unknown token in move text at '%1!'").arg(QString::number(stream->pos())));
                return false;
            }
        }
        if (stream->lookAhead(1) == PgnToken::LeftBrack)
            return true; //end of the movetext

        stream->next();
    }
    return true;
}

bool PgnParser::parseMove(PgnTokenStream *stream, Move *move)
{
//     qDebug() << "token:" << stream->token() << "text:"  << stream->text() << endl;
    QString err;
    bool ok = false;
    *move = Notation::stringToMove(stream->text(), Standard, &ok, &err);
    if (!err.isEmpty())
        emit error(err);
    return ok;
//    return true;
}

Game::Result PgnParser::parseResult(const QString &result)
{
    if (result == "1-0")
        return Game::WhiteWins;
    else if (result == "0-1")
        return Game::BlackWins;
    else if (result == "1/2-1/2")
        return Game::Drawn;
    return Game::NoResult;
}

void PgnParser::lexProgressOut(qint64 pos, qint64 size)
{
    qDebug() << "lexing..." << pos << "of" << size << endl;
}

void PgnParser::parseProgressOut(qint64 bytesReceived, qint64 bytesTotal)
{
    qDebug() << "parsing..." << bytesReceived << "of" << bytesTotal << endl;
}
