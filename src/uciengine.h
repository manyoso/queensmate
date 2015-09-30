#ifndef UCIENGINE_H
#define UCIENGINE_H

#include "engine.h"

#include <QProcess>
#include <QVariant>

class UciRegister {
};

class UciPosition {
};

class UciGo {
};

struct UciOption {
public:
    enum OptionType { Check, Spin, Combo, Button, String };
    enum Parameter { Name, Type, Default, Min, Max, Var };

    UciOption();
    ~UciOption();

    QVariant value() const;
    void setValue(const QVariant &value) const;

    QString optionName() const { return m_name; }
    OptionType optionType() const { return m_type; }
    QVariant optionDefault() const { return m_default; }
    QVariant optionMin() const { return m_min; }
    QVariant optionMax() const { return m_max; }
    QList<QVariant> optionVar() const { return m_var; }

    QString toString() const;

private:
    void setParameterValues(UciOption::Parameter parameter, QList<QVariant> values);

private:
    QString m_name;
    OptionType m_type;
    QVariant m_default;
    QVariant m_min;
    QVariant m_max;
    QList<QVariant> m_var;
    QVariant m_value;
    friend class UciEngine;
};

class UciEngine : public Engine {
    Q_OBJECT
public:
    UciEngine(const QString &fileName, Game *parent);
    ~UciEngine();

    //inherited from Engine
    virtual QString fileName() const { return m_fileName; }
    virtual QString engineName() const { return m_engineName; }
    virtual Protocol protocol() const { return UciProtocol; }

    //inherited from Player
    virtual void startGame();
    virtual void endGame();
    virtual void makeNextMove() const;
    virtual bool isReady() const { return false; }

    //get the option list
    static QList<UciOption> optionList(const QString &fileName);
    static QString engineName(const QString &fileName);

    //status of uci
    bool isUciOk() const { return m_isUciOk; }
    bool isReadyOk() const { return m_isReadyOk; }

    QList<UciOption> options() const { return m_options; }
    QString engineAuthor() const { return m_engineAuthor; }

public Q_SLOTS:
    void sendUci() const;
    void sendDebug(bool debug) const;
    void sendIsReady() const;
    void sendSetOption(const QString &name, const QVariant &value) const;
    void sendRegister();
    void sendUciNewGame() const;
    void sendPosition(const QString &position) const;
    void sendGo() const;
    void sendStop() const;
    void sendPonderHit() const;
    void sendQuit() const;

Q_SIGNALS:
    void receivedName(const QString &name);
    void receivedAuthor(const QString &author);
    void receivedUciOk() const;
    void receivedReadyOk() const;
    void receivedBestMove(const QString &move, const QString &ponder) const;
    void receivedCopyProtection() const;
    void receivedRegistration() const;
    void receivedInfo() const;
    void receivedOption() const;

private Q_SLOTS:
    void readyReadStandardError();
    void readyReadStandardOutput();
    void stateChanged(QProcess::ProcessState);
    void started();
    void error(QProcess::ProcessError);

private:
    void parseId(const QByteArray &line);
    void parseUciOk(const QByteArray &line);
    void parseReadyOk(const QByteArray &line);
    void parseBestMove(const QByteArray &line);
    void parseCopyProtection(const QByteArray &line);
    void parseRegistration(const QByteArray &line);
    void parseInfo(const QByteArray &line);
    void parseOption(const QByteArray &line);
    void parseError(const QByteArray &line);

private:
    bool waitForFinished(int msecs = 30000);
    bool waitForReadyRead(int msecs = 30000);

private:
    QProcess *m_process;
    QString m_fileName;
    QString m_engineName;
    QString m_engineAuthor;
    bool m_isUciOk;
    bool m_isReadyOk;
    QList<UciOption> m_options;
};

#endif
