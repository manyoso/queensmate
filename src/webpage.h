#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebPage>
#include <QWebFrame>

typedef QList<QPair<QString, QString> > QueryItemList;

class WebPage : public QWebPage {
    Q_OBJECT
public:
    WebPage(QObject *parent);
    ~WebPage();

protected:
    virtual bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type);
    virtual QString userAgentForUrl(const QUrl& url) const;

private Q_SLOTS:
    void handleUnsupportedContent(QNetworkReply *reply);
    void handleQuery(const QueryItemList &list);
};

#endif
