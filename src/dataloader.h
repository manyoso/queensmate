#ifndef DATALOADER_H
#define DATALOADER_H

#include <QObject>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>

class DataLoader : public QObject {
    Q_OBJECT
public:
    DataLoader(QObject *parent);
    ~DataLoader();

    void loadDataFromPath(const QString &path);

Q_SIGNALS:
    void progress(qint64 bytesReceived, qint64 bytesTotal);
    void error(const QString &error);
    void finished(const QByteArray &array);

private Q_SLOTS:
    void replyFinished(QNetworkReply *reply);
    void networkError(QNetworkReply::NetworkError code);

private:
    void loadFromDisk(const QString &path);
    void loadFromInternet(const QString &path);

private:
    QNetworkAccessManager *m_manager;
};

#endif
