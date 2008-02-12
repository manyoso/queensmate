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

    QTime baseTime() const { return m_baseTime; }
    void setBaseTime(const QTime &time)
    {
        m_baseTime = time;
        m_whiteTime = qAbs(m_baseTime.msecsTo(QTime()));
        m_blackTime = qAbs(m_baseTime.msecsTo(QTime()));
    }

    int moves() const { return m_moves; }
    void setMoves(int moves) { m_moves = moves; }

    QTime increment() const { return m_increment; }
    void setIncrement(const QTime &time) { m_increment = time; }

    QTime currentClock(Chess::Army army) const;

    int timeLeft(Chess::Army army) const; //msecs
    int increment(Chess::Army army) const; //msecs

    void startClock(Chess::Army army);
    void endClock();

Q_SIGNALS:
    void tick();
    void flagFell(Chess::Army army);

private Q_SLOTS:
    void updateSignals();

private:
    bool m_started;
    QTime m_baseTime;
    int m_moves;
    QTime m_increment;
    Chess::Army m_army;

    int m_whiteMoves;
    int m_whiteTime;

    int m_blackMoves;
    int m_blackTime;

    QTime m_clock;
    QTimer *m_timer;
};

#endif
