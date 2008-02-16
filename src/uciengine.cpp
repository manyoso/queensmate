#include "uciengine.h"

#include <QDebug>
#include <QVariant>

#include "chess.h"
#include "move.h"
#include "clock.h"
#include "notation.h"

using namespace Chess;

UciOption::UciOption()
    : m_name(QString()),
      m_type(String),
      m_default(QVariant()),
      m_min(QVariant()),
      m_max(QVariant()),
      m_var(QList<QVariant>()),
      m_value(QVariant())
{
}

UciOption::~UciOption()
{
}

void UciOption::setParameterValues(UciOption::Parameter parameter, QList<QVariant> values)
{
    switch (parameter) {
    case Name:
        {
            QStringList list;
            foreach (QVariant v, values) {
                list << v.toString();
            }
            m_name = list.join(" ");
            break;
        }
    case Type:
        {
            QString type = values.first().toString();
            if (type == "check")
                m_type = Check;
            else if (type == "spin")
                m_type = Spin;
            else if (type == "combo")
                m_type = Combo;
            else if (type == "button")
                m_type = Button;
            else if (type == "string")
                m_type = String;
            else
                Q_ASSERT(false);
        break;
        }
    case Default:
        m_default = values.first();
        break;
    case Min:
        m_min = values.first();
        break;
    case Max:
        m_max = values.first();
        break;
    case Var:
        m_var = values;
        break;
    default:
        break;
    }
}


QString UciOption::toString() const {
    QStringList list;
    list << "option" << "name" << m_name << "type";
    switch (m_type) {
    case Check:
        {
            list << "check";
            list << "default" << (m_default.toBool() ? "true" : "false");
            break;
        }
    case Spin:
        {
            list << "spin";
            list << "default" << m_default.toString();
            list << "min" << m_min.toString();
            list << "max" << m_max.toString();

            break;
        }
    case Combo:
        {
            list << "combo";
            list << "default" << m_default.toString();
            foreach (QVariant v, m_var) {
                list << "var" << v.toString();
            }
            break;
        }
    case Button:
        {
            list << "button";
            break;
        }
    case String:
        {
            list << "string";
            list << "default" << m_default.toString();
            break;
        }
    default:
        break;
    }
    return list.join(" ") +'\n';
}


UciEngine::UciEngine(const QString &fileName, Game *parent)
    : Engine(parent),
      m_fileName(fileName),
      m_isUciOk(false),
      m_isReadyOk(false)
{
    m_process = new QProcess(this);

    connect(m_process, SIGNAL(readyReadStandardError()),
            this, SLOT(readyReadStandardError()));
    connect(m_process, SIGNAL(readyReadStandardOutput()),
            this, SLOT(readyReadStandardOutput()));
    connect(m_process, SIGNAL(stateChanged(QProcess::ProcessState)),
            this, SLOT(stateChanged(QProcess::ProcessState)));
    connect(m_process, SIGNAL(started()),
            this, SLOT(started()));
    connect(m_process, SIGNAL(error(QProcess::ProcessError)),
            this, SLOT(error(QProcess::ProcessError)));

    //qDebug() << "attempting to start uci engine file" << m_fileName << endl;

#ifdef Q_OS_WIN
    m_process->start(QString("\"%1\"").arg(m_fileName));
#else
    m_process->start(m_fileName);
#endif

    m_process->waitForStarted();

    sendUci();
    sendIsReady();
}

UciEngine::~UciEngine()
{
    sendQuit();
    m_process->waitForFinished();
    m_process->close();
}

void UciEngine::startGame()
{
    sendUciNewGame();
}

void UciEngine::endGame()
{
}

void UciEngine::makeNextMove() const
{
    sendPosition(game()->fen(game()->position()));
    sendGo();
}

QList<UciOption> UciEngine::optionList(const QString &fileName)
{
    UciEngine engine(fileName, 0);
    engine.sendQuit();
    if (engine.waitForFinished()) {
        return engine.options();
    } else {
        return QList<UciOption>();
    }
}

QString UciEngine::engineName(const QString &fileName)
{
    UciEngine engine(fileName, 0);
    engine.sendQuit();
    if (engine.waitForFinished()) {
        return engine.engineName();
    } else {
        return QString();
    }
}

bool UciEngine::waitForFinished(int msecs)
{
    return m_process->waitForFinished(msecs);
}

bool UciEngine::waitForReadyRead(int msecs)
{
    return m_process->waitForReadyRead(msecs);
}

void UciEngine::readyReadStandardError()
{
    //qDebug() << "UciEngine::readyReadStandardError" << m_process->readAllStandardError() << endl;

    //while (m_process->canReadLine()) {
    //    QByteArray line = m_process->readLine();
    //    qDebug() << line << endl;
    //}
}

void UciEngine::readyReadStandardOutput()
{
    //qDebug() << "UciEngine::readyReadStandardOutput" << endl;

    while (m_process->canReadLine()) {
        QByteArray line = m_process->readLine();
        //qDebug() << line << endl;
        if (line.startsWith("id"))
            parseId(line.trimmed());
        else if (line.startsWith("uciok"))
            parseUciOk(line.trimmed());
        else if (line.startsWith("readyok"))
            parseReadyOk(line.trimmed());
        else if (line.startsWith("bestmove"))
            parseBestMove(line.trimmed());
        else if (line.startsWith("copyprotection"))
            parseCopyProtection(line.trimmed());
        else if (line.startsWith("registration"))
            parseRegistration(line.trimmed());
        else if (line.startsWith("info"))
            parseInfo(line.trimmed());
        else if (line.startsWith("option"))
            parseOption(line.trimmed());
        else
            parseError(line.trimmed());
    }
}

void UciEngine::stateChanged(QProcess::ProcessState state)
{
    Q_UNUSED(state);
    qDebug() << "UciEngine::stateChanged" << state << endl;
}

void UciEngine::started()
{
    //qDebug() << "UciEngine::started" << endl;
}

void UciEngine::error(QProcess::ProcessError error)
{
    qDebug() << "UciEngine::error" << error << endl;
}

void UciEngine::parseError(const QByteArray &line) {
    Q_UNUSED(line);
    //qDebug() << "Cound not parse!" << line << endl;
}

void UciEngine::parseId(const QByteArray &line)
{
    //qDebug() << "UciEngine::parseId" << line << endl;
    QByteArray signal = line;
    if (line.startsWith("id name ")) {
        m_engineName = signal.replace("id name ", "");
        emit receivedName(m_engineName);
    } else if (line.startsWith("id author ")) {
        m_engineAuthor = signal.replace("id author ", "");
        emit receivedName(m_engineAuthor);
    } else {
        parseError(line);
    }
}

void UciEngine::parseUciOk(const QByteArray &line)
{
    Q_UNUSED(line);
    //qDebug() << "UciEngine::parseUciOk" << line << endl;
    m_isUciOk = true;
    emit receivedUciOk();
}

void UciEngine::parseReadyOk(const QByteArray &line)
{
    Q_UNUSED(line);
    //qDebug() << "UciEngine::parseReadyOk" << line << endl;
    m_isReadyOk = true;
    emit receivedReadyOk();
    emit ready();
}

void UciEngine::parseBestMove(const QByteArray &line)
{
    //qDebug() << "UciEngine::parseBestMove" << line << endl;
    QList<QByteArray> bestMove = line.split(' ');
    if (bestMove.count() == 4) {
        emit receivedBestMove(bestMove[1], bestMove[3]);
        emit madeMove(Notation::stringToMove(bestMove[1], Chess::Computer));
    } else if (bestMove.count() == 2) {
        emit receivedBestMove(bestMove[1], QString());
        emit madeMove(Notation::stringToMove(bestMove[1], Chess::Computer));
    } else
        parseError(line);
}

void UciEngine::parseCopyProtection(const QByteArray &line)
{
    Q_UNUSED(line);
    //qDebug() << "UciEngine::parseCopyProtection" << line << endl;
}

void UciEngine::parseRegistration(const QByteArray &line)
{
    Q_UNUSED(line);
    //qDebug() << "UciEngine::parseRegistration" << line << endl;
}

void UciEngine::parseInfo(const QByteArray &line)
{
    Q_UNUSED(line);
    //qDebug() << "UciEngine::parseInfo" << line << endl;
}

void UciEngine::parseOption(const QByteArray &line)
{
    //qDebug() << "UciEngine::parseOption" << line << endl;
    QList<QByteArray> strings = line.split(' ');

    UciOption option;
    UciOption::Parameter state;
    QList<QVariant> values;

    foreach (QByteArray string, strings) {
    if (string == "option") {
        values = QList<QVariant>();
    } else if (string == "name") {
        option.setParameterValues(state, values);
        state = UciOption::Name;
        values = QList<QVariant>();
    } else if (string == "type") {
        option.setParameterValues(state, values);
        state = UciOption::Type;
        values = QList<QVariant>();
    } else if (string == "default") {
        option.setParameterValues(state, values);
        state = UciOption::Default;
        values = QList<QVariant>();
    } else if (string == "min") {
        option.setParameterValues(state, values);
        state = UciOption::Min;
        values = QList<QVariant>();
    } else if (string == "max") {
        option.setParameterValues(state, values);
        state = UciOption::Max;
        values = QList<QVariant>();
    } else if (string == "var") {
        option.setParameterValues(state, values);
        state = UciOption::Var;
        values = QList<QVariant>();
    } else {
        values << QString(string);
    }
    }

    option.setParameterValues(state, values);

    m_options << option;
}

void UciEngine::sendUci() const
{
    //qDebug() << "UciEngine::sendUci" << endl;
    m_process->write("uci\n");
}

void UciEngine::sendDebug(bool debug) const
{
    //qDebug() << "UciEngine::sendDebug"
    //         << "debug:" << (debug ? "true" : "false")
    //         << endl;
    m_process->write(QString("sendebug %1 \n").arg((debug ? "on" : "off")).toLatin1().constData());
}

void UciEngine::sendIsReady() const
{
    //qDebug() << "UciEngine::sendIsReady" << endl;
    m_process->write("isready\n");
}

void UciEngine::sendSetOption(const QString &name, const QVariant &value) const
{
    //qDebug() << "UciEngine::sendSetOption"
    //         << "name:" << name
    //         << "value:" << value.toString()
    //         << endl;
    m_process->write(QString("setoption name %1 value %2\n").arg(name).arg(value.toString()).toLatin1().constData());
}

void UciEngine::sendRegister()
{
    //qDebug() << "UciEngine::sendRegister" << endl;
}

void UciEngine::sendUciNewGame() const
{
    //qDebug() << "UciEngine::sendUciNewGame" << endl;
    m_process->write("ucinewgame\n");
}

void UciEngine::sendPosition(const QString &position) const
{
    qDebug() << "UciEngine::sendPosition:" << position << endl;
    m_process->write(QString("position fen %1\n").arg(position).toLatin1());
}

void UciEngine::sendGo() const
{
    //qDebug() << "UciEngine::sendGo" << endl;
    //FIXME Need to figure out all the options...

    QString wtime = QString(" wtime %1").arg(QString::number(game()->clock()->timeLeft(White)));
    if (game()->clock()->isUnlimited(White))
        wtime = QString(); //FIXME should this be a very large integer instead??
    QString btime = QString(" btime %1").arg(QString::number(game()->clock()->timeLeft(Black)));
    if (game()->clock()->isUnlimited(Black))
        btime = QString(); //FIXME should this be a very large integer instead??

    QString winc = QString(" winc %1").arg(QString::number(game()->clock()->incrementLeft(White)));
    if (game()->clock()->incrementLeft(White) < 1)
        winc = QString();
    QString binc = QString(" binc %1").arg(QString::number(game()->clock()->incrementLeft(Black)));
    if (game()->clock()->incrementLeft(Black) < 1)
        binc = QString();

    QString movestogo = QString(" movestogo %1").arg(QString::number(game()->clock()->moves(army())));
    if (game()->clock()->moves(army()) < 1)
        movestogo = QString();

    qDebug() << QString("go%1%2%3%4%5\n").arg(wtime).arg(btime).arg(winc).arg(binc).arg(movestogo).toLatin1() << endl;
    m_process->write(QString("go%1%2%3%4%5\n").arg(wtime).arg(btime).arg(winc).arg(binc).arg(movestogo).toLatin1());
}

void UciEngine::sendStop() const
{
    //qDebug() << "UciEngine::sendStop" << endl;
    m_process->write("stop\n");
}

void UciEngine::sendPonderHit() const
{
    //qDebug() << "UciEngine::sendPonderHit" << endl;
    m_process->write("ponderhit\n");
}

void UciEngine::sendQuit() const
{
    //qDebug() << "UciEngine::sendQuit" << endl;
    m_process->write("quit\n");
}
