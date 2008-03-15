#ifndef CLOCK_H
#define CLOCK_H

#include <QObject>

#include <QTime>
#include <QTimer>

#include "chess.h"

class Clock : public QObject {
    Q_OBJECT
public:
    Clock(QObject *parent);
    ~Clock();

    QTime baseTime(Chess::Army army) const;
    void setBaseTime(Chess::Army army, const QTime &time);

    QTime increment(Chess::Army army) const;
    void setIncrement(Chess::Army army, const QTime &time);

    int moves(Chess::Army army) const;
    void setMoves(Chess::Army army, int moves);

    bool isUnlimited(Chess::Army army) const;
    void setUnlimited(Chess::Army army, bool isUnlimited);

    QTime currentClock(Chess::Army army) const;

    int timeLeft(Chess::Army army) const; //msecs
    int incrementLeft(Chess::Army army) const; //msecs

    void startClock(Chess::Army army);
    void endClock();

    void reset();

Q_SIGNALS:
    void tick();
    void flagFell(Chess::Army army);
    void onTheClock(Chess::Army army);

private Q_SLOTS:
    void updateSignals();

private:
    bool m_started;

    Chess::Army m_army;

    int m_whiteTime;
    QTime m_whiteBaseTime;
    QTime m_whiteIncrement;
    int m_whiteMoves;
    bool m_whiteIsUnlimited;

    int m_blackTime;
    QTime m_blackBaseTime;
    QTime m_blackIncrement;
    int m_blackMoves;
    bool m_blackIsUnlimited;

    QTime m_clock;
    QTimer *m_timer;
};

#endif
