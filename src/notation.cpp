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
// d8        //Pawn to d8
// cxd8      //Pawn on c captures d8
// cxd8=Q+   //Pawn on c captures d8 promotes to queen and check
// Qcd8      //Queen on file c to d8
// Qc8d8     //Queen on file c and rank 8 to d8
// Qxd8      //Queen captures d8
// Qcxd8     //Queen on file c captures d8
// Qc8xd8    //Queen on file c and rank 8 captures d8
// Qc8xd8+   //Queen on file c and rank 8 captures d8 check

            QString str = string;
            if (str.contains('x')) {
                move.setCapture(true);
                str = str.remove('x');
            }

            if (str.contains('=')) {
                int i = str.indexOf('=');
                QChar c = str.at(i + 1);
                move.setPromotion(charToPiece(c, notation));
                str = str.remove(i + 1, 1);
                str = str.remove('=');
            }

            if (str.contains('+')) {
                move.setCheck(true);
                str = str.remove('+');
            }
            if (str.contains('#')) {
                move.setCheckMate(true);
                str = str.remove('#');
            }

            if (str == "O-O") {
                move.setPiece(King);
                move.setCastle(true);
                move.setCastleSide(KingSide);
                break;
            } else if (str == "O-O-O") {
                move.setPiece(King);
                move.setCastle(true);
                move.setCastleSide(QueenSide);
                break;
            } else if (str == "0-1") {
                break;
            } else if (str == "1-0") {
                break;
            } else if (str == "1/2-1/2") {
                break;
            }

            if (str.length() == 2) {
                move.setPiece(Pawn);
                move.setEnd(stringToSquare(str.right(2), notation));
            } else if (str.length() == 3) {
                QChar c = str.at(0);
                if (c.isUpper()) {
                    move.setPiece(charToPiece(c, notation));
                } else if (c.isLower()) {
                    move.setPiece(Pawn);
                    move.setFileOfDeparture(charToFile(c, notation));
                }
                move.setEnd(stringToSquare(str.right(2), notation));
            } else if (str.length() == 4) {
                move.setPiece(charToPiece(str.at(0), notation));
                QChar c = str.at(1);
                if (c.isLetter() && c.isLower()) {
                    move.setFileOfDeparture(charToFile(c, notation));
                } else if (c.isNumber()) {
                    move.setRankOfDeparture(charToRank(c, notation));
                }
                move.setEnd(stringToSquare(str.right(2), notation));
            } else if (str.length() == 5) {
                move.setPiece(charToPiece(str.at(0), notation));
                move.setFileOfDeparture(charToFile(str.at(1), notation));
                move.setRankOfDeparture(charToRank(str.at(2), notation));
                move.setEnd(stringToSquare(str.right(2), notation));
            }
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
                    str += fileToChar(move.fileOfDeparture());
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

    Q_ASSERT_X(file >=0 && file <= 7,
               "Notation::fileToChar(int file, ...) range error",
               QString("%1").arg(QString::number(file)).toLatin1().constData());

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
