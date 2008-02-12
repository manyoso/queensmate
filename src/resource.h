#ifndef RESOURCE_H
#define RESOURCE_H

#include <QObject>
#include <QStringList>

class Resource : public QObject {

Q_OBJECT

public:
    Resource(QObject *parent);
    ~Resource();

    QString fenFor960(int i = -1) const;

private:
    QStringList m_fenFor960;
};

#endif
