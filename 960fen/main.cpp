#include <QDebug>
#include <QString>
#include <QStringList>

#include <math.h>
#include <iostream>

/*
 * Uses method of R. Scharnagl to generate starting FEN
 * for all possible games of Chess960/Fischer Random Chess.
 */

int main(int argc, char *argv[])
{
    Q_UNUSED(argc);
    Q_UNUSED(argv);

    for (int id = 0; id < 960; id++) {

        QString startingLine(8, ' ');

        /*
         * Divide the id by 4, producing a truncated integer and a remainder.
         * The remainder locates the light-square Bishop: 0 means file b,
         * 1 means file d, 2 means file f, and 3 means file h.
         */
        int a = (int)floor(qreal(id / 4));
        int b = id % 4;

        if (b == 0)
            startingLine[1] = 'B';
        else if (b == 1)
            startingLine[3] = 'B';
        else if (b == 2)
            startingLine[5] = 'B';
        else if (b == 3)
            startingLine[7] = 'B';

        /*
         * Take the previous truncated integer and divide by 4, producing
         * another integer and a remainder. This remainder locates the
         * dark-square Bishop: 0 means file a, 1 means file c, 2 means file e,
         * and 3 means file g.
         */
        int c = (int)floor(qreal(a / 4));
        int d = a % 4;

        if (d == 0)
            startingLine[0] = 'B';
        else if (d == 1)
            startingLine[2] = 'B';
        else if (d == 2)
            startingLine[4] = 'B';
        else if (d == 3)
            startingLine[6] = 'B';

        /*
         * Take the previous truncated integer and divide by 6, producing
         * another integer and a remainder. This remainder locates the queen,
         * and identifies the number of the vacant square it occupies (counting
         * from the left, where 0 is the leftmost square and 5 is the rightmost
         * square).
         */
        int e = (int)floor(qreal(c / 6));
        int f = c % 6;

        int countF = 0;
        for (int i = 0; i < 8; i++) {
            if (startingLine[i] == ' ') {
                if (countF++ == f) {
                    startingLine[i] = 'Q';
                    break;
                }
            }
        }

        /*
         * The previous truncated integer now has a value from 0 to 9 inclusive.
         * Its value, called the KRN code (pronounced "kern"), indicates the
         * positions of the king, rooks, and knights among the remaining 5 squares.
         */
        Q_ASSERT(0 <= e && e <= 9);

        QStringList kernList;
        kernList << "N N R K R";
        kernList << "N R N K R";
        kernList << "N R K N R";
        kernList << "N R K R N";
        kernList << "R N N K R";
        kernList << "R N K N R";
        kernList << "R N K R N";
        kernList << "R K N N R";
        kernList << "R K N R N";
        kernList << "R K R N N";

        QStringList kern = kernList.at(e).split(" ");

        QList<int> rooks;

        int countK = 0;
        for (int i = 0; i < 8; i++) {
            if (startingLine[i] == ' ') {
                QChar c = kern.at(countK++).at(0);
                if (c == 'R')
                    rooks << i;
                startingLine[i] = c;
            }
        }

        QList<QChar> files;
        files << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h';
        QString rookFiles = QString("%1%2").arg(files.at(rooks.at(0))).arg(files.at(rooks.at(1)));

        QStringList fenFields;
        fenFields << startingLine.toLower();
        fenFields << "pppppppp";
        fenFields << "8";
        fenFields << "8";
        fenFields << "8";
        fenFields << "8";
        fenFields << "PPPPPPPP";
        fenFields << startingLine.toUpper();

        QString fen = fenFields.join("/") + QString(" w %1%2 - 0 1").arg(rookFiles.toUpper()).arg(rookFiles.toLower());

        std::cout << fen.toLatin1().constData()
                  << "\n";
    }
}
