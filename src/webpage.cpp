#include "webpage.h"

#include <QDebug>
#include <QNetworkRequest>
#include <QDesktopServices>

#include "application.h"

WebPage::WebPage(QObject *parent)
    : QWebPage(parent)
{
    connect(this, SIGNAL(linkHovered(const QString &, const QString &, const QString &)),
            chessApp, SLOT(showStatus(const QString &)));
    connect(this, SIGNAL(statusBarMessage(const QString &)),
            chessApp, SLOT(showStatus(const QString &)));
}

WebPage::~WebPage()
{
}

bool WebPage::acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type)
{
    Q_UNUSED(frame);
    switch (type) {
    case QWebPage::NavigationTypeLinkClicked:
        {
            if (request.url().host() == chessApp->url().host()) {
                return true;
            } else {
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
