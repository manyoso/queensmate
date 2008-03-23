#ifndef PGN_H
#define PGN_H

#include <QHash>
#include <QString>

class Pgn {
public:
    Pgn();
    ~Pgn();

private:
    QString m_event;
    QString m_site;
    QString m_date;
    QString m_round;
    QString m_white;
    QString m_black;
    QString m_result;
    QHash<QString, QString> m_tags;
    friend class PgnParser;
};

#endif
