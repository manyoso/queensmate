#include "notation.h"

#include "move.h"
#include "square.h"

#include <QList>
#include <QDebug>

using namespace Chess;

Move Notation::stringToMove(const QString &string, Chess::NotationType notation, bool *ok, QString *err)
{
    //FIXME
    Q_UNUSED(ok);
    Q_UNUSED(err);

    //qDebug() << "Notation::stringToMove:" << string << endl;

    Move move;

    switch (notation) {
    case Standard:
        {
            move.setCapture(string.contains('x'));

            if (string.contains('=')) {
                ;
            }

            move.setCheck(string.contains('+'));
            move.setCheckMate(string.contains('#'));

            if (string == "O-O") {
                move.setCastle(true);
                move.setCastleSide(KingSide);
            } else if (string == "O-O-0") {
                move.setCastle(true);
                move.setCastleSide(QueenSide);
            }

            move.setPiece(charToPiece(string.at(0), notation));
            move.setEnd(stringToSquare(string.right(2), notation));
            break;
        }
    case Long:
            move.setCapture(string.contains('x'));
            move.setPiece(charToPiece(string.at(0), notation));
            move.setStart(move.piece() != Pawn ? stringToSquare(string.mid(1, 2), notation) :
                                                 stringToSquare(string.left(2), notation));
            move.setEnd(stringToSquare(string.right(2), notation));
            break;
    case Computer:
        {
            if (string == QLatin1String("(none)")) //glaurang sends this...
                break;

            move.setStart(stringToSquare(string.left(2), notation));
            move.setEnd(stringToSquare(string.mid(2, 2), notation));
            if (string.size() == 5) { //promotion
                move.setPromotion(charToPiece(string.at(4), notation));
            }
            break;
        }
    default:
        break;
    }

    return move;
}

QString Notation::moveToString(Move move, Chess::NotationType notation)
{
    QString str;

    switch (notation) {
    case Standard:
        {
            QChar piece = pieceToChar(move.piece(), notation);
            QChar capture = move.isCapture() ? 'x' : QChar();
            QChar check = move.isCheck() ? '+' : QChar();
            QChar checkMate = move.isCheckMate() ? '#' : QChar();
            QString square = squareToString(move.end(), notation);

            if (!piece.isNull()) {
                str += piece;
            }

            if (!capture.isNull()) {
                if (move.piece() == Pawn) {
                    str += fileToChar(move.start().file());
                }
                str += capture;
            }

            str += square;

            if (move.promotion() != Unknown) {
                str += QString("=%1").arg(pieceToChar(move.promotion()));
            }

            if (!checkMate.isNull()) {
                str += checkMate;
            } else if (!check.isNull()) {
                str += check;
            }

            if (move.isCastle()) {
                if (move.isKingSideCastle())
                    str = "O-O";
                else if (move.isQueenSideCastle())
                    str = "O-O-O";
            }

            break;
        }
    case Long:
        {
            QChar piece = pieceToChar(move.piece(), notation);
            QChar sep = move.isCapture() ? 'x' : '-';
            QString start = squareToString(move.start(), notation);
            QString end = squareToString(move.end(), notation);
            if (!piece.isNull())
                str += piece;

            str += start;
            str += sep;
            str += end;
            break;
        }
    case Computer:
        {
            QString start = squareToString(move.start(), notation);
            QString end = squareToString(move.end(), notation);
            str += start;
            str += end;
        break;
        }
    default:
        break;
    }

    return str;
}

Square Notation::stringToSquare(const QString &string, Chess::NotationType notation, bool *ok, QString *err)
{
    //FIXME
    Q_UNUSED(ok);
    Q_UNUSED(err);

    int file;
    int rank;

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            Q_ASSERT(string.size() == 2);
            file = charToFile(string.at(0), notation, ok, err);
            rank = charToRank(string.at(1), notation, ok, err);
            break;
        }
    default:
        break;
    }

    return Square(file, rank);
}

QString Notation::squareToString(Square square, Chess::NotationType notation)
{
    if (!square.isValid())
        return QString();

    QString str;

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            str = QString("%1%2").arg(fileToChar(square.file())).arg(rankToChar(square.rank()));
            break;
        }
    default:
        break;
    }

    return str;
}

Chess::PieceType Notation::charToPiece(const QChar &ch, Chess::NotationType notation, bool *ok, QString *err)
{
    //FIXME
    Q_UNUSED(ok);
    Q_UNUSED(err);

    PieceType piece;

    QList<QChar> pieces;
    pieces << 'U' << 'K' << 'Q' << 'R' << 'B' << 'N' << 'P';

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            if (pieces.contains(ch.toUpper())) {
                piece = PieceType(pieces.indexOf(ch.toUpper()));
            } else {
                piece = Pawn;
            }
            break;
        }
    default:
        break;
    }

    return piece;
}

QChar Notation::pieceToChar(Chess::PieceType piece, Chess::NotationType notation)
{
    QChar ch;

    QList<QChar> pieces;
    pieces << 'U' << 'K' << 'Q' << 'R' << 'B' << 'N' << 'P';

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            ch = pieces.at(piece);
            if (ch == 'P')
                ch = QChar();
            break;
        }
    default:
        break;
    }

    return ch;
}

int Notation::charToFile(const QChar &ch, Chess::NotationType notation, bool *ok, QString *err)
{
    //FIXME
    Q_UNUSED(ok);
    Q_UNUSED(err);

    int file;

    QList<QChar> files;
    files << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h';

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            if (files.contains(ch.toLower())) {
                file = files.indexOf(ch.toLower());
            } else {
                file = -1;
            }
            break;
        }
    default:
        break;
    }

    return file;
}

QChar Notation::fileToChar(int file, Chess::NotationType notation)
{
    QChar ch;

    QList<QChar> files;
    files << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h';

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            ch = files.at(file);
            break;
        }
    default:
        break;
    }

    return ch;
}

int Notation::charToRank(const QChar &ch, Chess::NotationType notation, bool *ok, QString *err)
{
    //FIXME
    Q_UNUSED(ok);
    Q_UNUSED(err);

    int rank;

    QList<QChar> ranks;
    ranks << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8';

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            if (ranks.contains(ch.toLower())) {
                rank = ranks.indexOf(ch.toLower());
            } else {
                rank = -1;
            }
            break;
        }
    default:
        break;
    }

    return rank;
}

QChar Notation::rankToChar(int rank, Chess::NotationType notation)
{
    QChar ch;

    QList<QChar> ranks;
    ranks << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8';

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            ch = ranks.at(rank);
            break;
        }
    default:
        break;
    }

    return ch;
}

Notation::Notation()
{
}

Notation::~Notation()
{
}
