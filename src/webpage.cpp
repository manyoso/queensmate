#include "webpage.h"

#include <QDebug>
#include <QTimer>
#include <QNetworkRequest>
#include <QDesktopServices>

#include "mainwindow.h"
#include "application.h"

WebPage::WebPage(MainWindow *mainWindow, QObject *parent)
    : QWebPage(parent)
{
    connect(this, SIGNAL(linkHovered(const QString &, const QString &, const QString &)),
            chessApp, SLOT(showStatus(const QString &)));
    connect(this, SIGNAL(statusBarMessage(const QString &)),
            chessApp, SLOT(showStatus(const QString &)));
    connect(this, SIGNAL(unsupportedContent(QNetworkReply *)),
            this, SLOT(handleUnsupportedContent(QNetworkReply *)));

    connect(this, SIGNAL(newGame()),
            mainWindow, SLOT(newGame()),
            Qt::QueuedConnection);
    connect(this, SIGNAL(loadGameFromPGN(const QString &)),
            mainWindow, SLOT(loadGameFromPGN(const QString &)),
            Qt::QueuedConnection);
    connect(this, SIGNAL(loadGameFromFEN(const QString &)),
            mainWindow, SLOT(loadGameFromFEN(const QString &)),
            Qt::QueuedConnection);
}

WebPage::~WebPage()
{
}

bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type)
{
    Q_UNUSED(frame);

    if (request.url().host() == chessApp->url().host()) {
        if (request.url().path() == QString("/%1").arg(QCoreApplication::applicationName())) {
            handleQuery(request.url().queryItems());
            return false;
        }
    }

    switch (type) {
    case QWebPage::NavigationTypeLinkClicked:
        {
            if (request.url().host() != chessApp->url().host()) {
                QDesktopServices::openUrl(request.url());
                return false;
            }
        }
    case QWebPage::NavigationTypeFormSubmitted:
    case QWebPage::NavigationTypeBackOrForward:
    case QWebPage::NavigationTypeReload:
    case QWebPage::NavigationTypeFormResubmitted:
    case QWebPage::NavigationTypeOther:
    default:
        break;
    }
    return QWebPage::acceptNavigationRequest(frame, request, type);
}

QString WebPage::userAgentForUrl(const QUrl& url) const
{
    Q_UNUSED(url);
    QString userAgent = QCoreApplication::applicationName() + '/' + QCoreApplication::applicationVersion();
    return userAgent;
}

void WebPage::handleUnsupportedContent(QNetworkReply *reply)
{
    Q_UNUSED(reply);
    Q_ASSERT(false);
}

void WebPage::handleQuery(const QueryItemList &list)
{
    QueryHash hash;
    QueryItemList::ConstIterator it = list.begin();
    for (; it != list.end(); ++it) {
        QString name = (*it).first;
        QString value = (*it).second;
        hash.insert(name, value);
    }

    if (!hash.contains("action"))
        return;

    QString action = hash.value("action");
    if (action == "newGame") {
        emit newGame();
    } else if (action == "loadGameFromPGN") {
        if (!hash.contains("url"))
            return;
        QString url = hash.value("url");
        emit loadGameFromPGN(url);
    } else if (action == "loadGameFromFEN") {
        if (!hash.contains("fen"))
            return;
        QString fen = hash.value("fen");
        emit loadGameFromFEN(fen);
    }
}
