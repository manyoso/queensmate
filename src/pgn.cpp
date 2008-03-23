#include "pgn.h"

#include <QDebug>

#include "chess.h"
#include "notation.h"

using namespace Chess;

Pgn::Pgn()
{
}

Pgn::~Pgn()
{
}

void Pgn::addTag(const QString &name, const QString &value)
{
    qDebug() << "addTag" << name << value << endl;
    m_tags.insert(name, value);
}

void Pgn::addMoveNumber(int number)
{
    qDebug() << "addMoveNumber" << number << endl;
    Q_UNUSED(number);
}

void Pgn::addMove(const Move &move)
{
//     if (move.piece() == Unknown /*result*/)
//         return;
    //qDebug() << "addMove" << Notation::moveToString(move) << endl;
    Q_UNUSED(move);
    m_moves.insert(m_moves.count() + 1, move);
}
