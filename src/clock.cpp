#include "clock.h"

#include <QDebug>

using namespace Chess;

Clock::Clock(QObject *parent)
    : QObject(parent),
      m_started(false),
      m_baseTime(QTime(0, 1)),
      m_moves(40),
      m_increment(QTime(0, 0, 0)),
      m_army(White),
      m_whiteMoves(0),
      m_blackMoves(0)
{
    m_clock = QTime();

    m_whiteTime = qAbs(m_baseTime.msecsTo(QTime()));
    m_blackTime = qAbs(m_baseTime.msecsTo(QTime()));

    m_timer = new QTimer(this);
    m_timer->setInterval(250);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateSignals()));
}

Clock::~Clock()
{
}

QTime Clock::currentClock(Chess::Army army) const
{
    QTime t;
    if (army == White)
        t = t.addMSecs(m_whiteTime);
    else
        t = t.addMSecs(m_blackTime);
    return t;
}

int Clock::timeLeft(Chess::Army army) const
{
    if (army == White)
        return m_whiteTime;
    else
        return m_blackTime;
}

int Clock::increment(Chess::Army army) const
{
    if (army == White)
        return qAbs(m_increment.msecsTo(QTime()));
    else
        return qAbs(m_increment.msecsTo(QTime()));
}

void Clock::startClock(Chess::Army army)
{
    m_army = army;

//     qDebug() << "startClock"
//              << "army" << army
//              << "m_whiteTime" << m_whiteTime
//              << "m_blackTime" << m_blackTime
//              << "m_clock" << m_clock.elapsed()
//              << endl;

    int elapsed = m_clock.restart();

    if (army == White) {
        if (m_started) {
            m_blackTime -= elapsed;
        }

        m_whiteTime += increment(army);
    } else {
        if (m_started) {
            m_whiteTime -= elapsed;
        }

        m_blackTime += increment(army);
    }

    if (!m_started) {
        m_timer->start();
        m_started = true;
    }

    emit tick();
}

void Clock::endClock()
{
    m_clock = QTime();
    m_timer->stop();
}

void Clock::updateSignals()
{
//     qDebug() << "updateSignals"
//              << "m_army" << m_army
//              << "m_whiteTime" << m_whiteTime
//              << "m_blackTime" << m_blackTime
//              << "m_clock" << m_clock.elapsed()
//              << endl;

    int elapsed = m_clock.restart();

    if (m_army == White) {
        m_whiteTime -= elapsed;
    } else {
        m_blackTime -= elapsed;
    }

    if (m_whiteTime <= 0) {
        qDebug() << "flagFell for white" << endl;
        endClock();
        emit flagFell(White);
        return;
    } else if (m_blackTime <= 0) {
        qDebug() << "flagFell for black" << endl;
        endClock();
        emit flagFell(Black);
        return;
    }

    emit tick();

    m_clock.restart();
}
