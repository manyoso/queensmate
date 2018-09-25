#include "dataloader.h"

#include <QUrl>
#include <QFile>
#include <QDebug>

DataLoader::DataLoader(QObject *parent)
    : QObject(parent)
{
    m_manager = new QNetworkAccessManager(this);
}

DataLoader::~DataLoader()
{
}

void DataLoader::loadDataFromPath(const QString &path)
{
    QFile file(path);
    if (file.exists()) {
        loadFromDisk(path);
    } else {
        loadFromInternet(path);
    }
}

void DataLoader::loadFromDisk(const QString &path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        emit error("Could not open file for reading!");
        return;
    }
    emit finished(file.readAll());
}

void DataLoader::loadFromInternet(const QString &path)
{
    QUrl url(path);
    if (!url.isValid()) {
        emit error("Url is invalid!");
        return;
    }

    QNetworkRequest request(url);
    QNetworkReply *reply = m_manager->get(request);
    connect(m_manager, SIGNAL(finished(QNetworkReply *)),
            this, SLOT(replyFinished(QNetworkReply *)));
    connect(reply, SIGNAL(downloadProgress(qint64, qint64)),
            this, SIGNAL(progress(qint64, qint64)));
    connect(reply, SIGNAL(error(QNetworkReply::NetworkError)),
            this, SLOT(networkError(QNetworkReply::NetworkError)));
}

void DataLoader::replyFinished(QNetworkReply *reply)
{
    if (reply->error() != QNetworkReply::NoError)
        return;
    emit finished(reply->readAll());
    reply->deleteLater();
    reply = 0;
}

void DataLoader::networkError(QNetworkReply::NetworkError code)
{
    emit error(QString("Network error code is %1").arg(QString::number(code)));
}
