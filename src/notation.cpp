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
            if (!piece.isNull())
                str += piece;
            if (!capture.isNull())
                str += capture;

            str += square;

            if (!checkMate.isNull())
                str += checkMate;
            else if (!check.isNull())
                str += check;
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

    QList<QChar> files;
    files << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h';
    QList<QChar> ranks;
    ranks << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8';

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            Q_ASSERT(string.size() == 2);
            file = files.indexOf(string.at(0));
            rank = ranks.indexOf(string.at(1));
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

    QList<QChar> files;
    files << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h';
    QList<QChar> ranks;
    ranks << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8';

    switch (notation) {
    case Standard:
    case Long:
    case Computer:
        {
            str = QString("%1%2").arg(files.at(square.file())).arg(ranks.at(square.rank()));
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

Notation::Notation()
{
}

Notation::~Notation()
{
}
