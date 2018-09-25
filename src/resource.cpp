#include "resource.h"

#include <QFile>
#include <QDebug>

#include <time.h>

Resource::Resource(QObject *parent)
    : QObject(parent)
{
    QFile file(":/960fen.txt");
    Q_ASSERT(file.open(QIODevice::ReadOnly | QIODevice::Text));

    while (!file.atEnd()) {
        QByteArray line = file.readLine();
        m_fenFor960 << line;
    }

    Q_ASSERT(m_fenFor960.count() == 960);

    srand(time(NULL));
}

Resource::~Resource()
{
}

QString Resource::fenFor960(int i) const
{
    if (i == -1)
        i = rand() % 959;

    Q_ASSERT(i >= 0);
    Q_ASSERT(i <= 959);
    Q_ASSERT(m_fenFor960.count() == 960);
    return m_fenFor960.at(i);
}
