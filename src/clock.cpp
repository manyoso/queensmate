#include "clock.h"

#include <QDebug>

using namespace Chess;

Clock::Clock(QObject *parent)
    : QObject(parent),
      m_started(false),
      m_army(White),
      m_whiteBaseTime(QTime(0, 0, 0)),
      m_whiteIncrement(QTime(0, 0, 0)),
      m_whiteMoves(-1),
      m_whiteIsUnlimited(false),
      m_blackBaseTime(QTime(0, 0, 0)),
      m_blackIncrement(QTime(0, 0, 0)),
      m_blackMoves(-1),
      m_blackIsUnlimited(false)
{
    m_clock = QTime();

    m_whiteTime = qAbs(m_whiteBaseTime.msecsTo(QTime()));
    m_blackTime = qAbs(m_blackBaseTime.msecsTo(QTime()));

    m_timer = new QTimer(this);
    m_timer->setInterval(250);
    connect(m_timer, SIGNAL(timeout()), this, SLOT(updateSignals()));
}

Clock::~Clock()
{
}

QTime Clock::baseTime(Chess::Army army) const
{
    if (army == White)
        return m_whiteBaseTime;
    else
        return m_blackBaseTime;
}

void Clock::setBaseTime(Chess::Army army, const QTime &time)
{
    if (army == White) {
        m_whiteBaseTime = time;
        m_whiteTime = qAbs(m_whiteBaseTime.msecsTo(QTime()));
    } else {
        m_blackBaseTime = time;
        m_blackTime = qAbs(m_blackBaseTime.msecsTo(QTime()));
    }
}

QTime Clock::increment(Chess::Army army) const
{
    if (army == White)
        return m_whiteIncrement;
    else
        return m_blackIncrement;
}

void Clock::setIncrement(Chess::Army army, const QTime &time)
{
    if (army == White)
        m_whiteIncrement = time;
    else
        m_blackIncrement = time;
}

int Clock::moves(Chess::Army army) const
{
    if (army == White)
        return m_whiteMoves;
    else
        return m_blackMoves;
}

void Clock::setMoves(Chess::Army army, int moves)
{
    if (army == White)
        m_whiteMoves = moves;
    else
        m_blackMoves = moves;
}

bool Clock::isUnlimited(Chess::Army army) const
{
    if (army == White)
        return m_whiteIsUnlimited;
    else
        return m_blackIsUnlimited;
}

void Clock::setUnlimited(Chess::Army army, bool isUnlimited)
{
    if (army == White)
        m_whiteIsUnlimited = isUnlimited;
    else
        m_blackIsUnlimited = isUnlimited;
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

int Clock::incrementLeft(Chess::Army army) const
{
    if (army == White)
        return qAbs(m_whiteIncrement.msecsTo(QTime()));
    else
        return qAbs(m_blackIncrement.msecsTo(QTime()));
}

void Clock::startClock(Chess::Army army)
{
    m_army = army;

    if (m_army == White && m_whiteBaseTime == QTime(0, 0, 0))
        return;

    if (m_army == Black && m_blackBaseTime == QTime(0, 0, 0))
        return;

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

        m_whiteTime += incrementLeft(army);
    } else {
        if (m_started) {
            m_whiteTime -= elapsed;
        }

        m_blackTime += incrementLeft(army);
    }

    if (m_whiteTime <= 0 && !m_whiteIsUnlimited) {
        qDebug() << "flagFell for white" << endl;
        m_whiteTime = 0;
        endClock();
        emit flagFell(White);
        emit tick();
        return;
    } else if (m_blackTime <= 0 && !m_blackIsUnlimited) {
        qDebug() << "flagFell for black" << endl;
        m_blackTime = 0;
        endClock();
        emit flagFell(Black);
        emit tick();
        return;
    }

    if (!m_started) {
        m_timer->start();
        m_started = true;
    }

    emit onTheClock(m_army);
    emit tick();
}

void Clock::endClock()
{
    m_clock = QTime();
    m_timer->stop();
}

void Clock::reset()
{
    m_started = false;
    m_army = White;
    m_whiteMoves = -1;
    m_blackMoves = -1;

    endClock();
    m_whiteTime = qAbs(m_whiteBaseTime.msecsTo(QTime()));
    m_blackTime = qAbs(m_blackBaseTime.msecsTo(QTime()));
    emit tick();
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

    if (m_whiteTime <= 0 && !m_whiteIsUnlimited) {
        qDebug() << "flagFell for white" << endl;
        m_whiteTime = 0;
        endClock();
        emit flagFell(White);
        emit tick();
        return;
    } else if (m_blackTime <= 0 && !m_blackIsUnlimited) {
        qDebug() << "flagFell for black" << endl;
        m_blackTime = 0;
        endClock();
        emit flagFell(Black);
        emit tick();
        return;
    }

    emit tick();

    m_clock.restart();
}
