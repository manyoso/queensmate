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
    Q_ASSERT_X(isValid(),
               "Square(int file, int rank)",
               QString("%1%2").arg(QString::number(file)).arg(QString::number(rank)).toLatin1().constData());
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
