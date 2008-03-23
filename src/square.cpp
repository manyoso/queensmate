#include "square.h"

#include <QDebug>

#include "notation.h"

Square::Square()
    : m_file(-1), m_rank(-1)
{
    Q_ASSERT(!isValid());
}

Square::Square(int file, int rank)
    : m_file(file), m_rank(rank)
{
}

Square::~Square()
{
}

int Square::index() const
{
    int f = m_file + 1;
    int r = m_rank + 1;
    return (8 * r) - (8 - f) - 1;
}

QDebug operator<<(QDebug debug, const Square &square)
{
    debug.nospace() << QString::number(square.file()) << ',' << QString::number(square.rank()) << "\n";
    return debug.space();
}
