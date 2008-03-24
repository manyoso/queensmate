#include "pgn.h"

#include <QDebug>

#include "chess.h"
#include "notation.h"

using namespace Chess;

Pgn::Pgn()
{
    qRegisterMetaType<PgnList>("PgnList");
}

Pgn::~Pgn()
{
}

QString Pgn::tag(const QString &name) const
{
    if (m_tags.contains(name))
        return m_tags.value(name);
    return QString();
}

void Pgn::addTag(const QString &name, const QString &value)
{
//     qDebug() << "addTag" << name << value << endl;
    m_tags.insert(name, value);
}

void Pgn::addMoveNumber(int number)
{
//     qDebug() << "addMoveNumber" << number << endl;
    Q_UNUSED(number);
}

void Pgn::addMove(const Move &move)
{
//     qDebug() << "addMove" << Notation::moveToString(move) << endl;
    m_moves.insert(m_moves.count() + 1, move);
}
