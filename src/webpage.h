#ifndef WEBPAGE_H
#define WEBPAGE_H

#include <QWebPage>
#include <QWebFrame>

class MainWindow;

typedef QHash<QString, QString> QueryHash;
typedef QList<QPair<QString, QString> > QueryItemList;

class WebPage : public QWebPage {
    Q_OBJECT
public:
    WebPage(MainWindow *mainWindow, QObject *parent);
    ~WebPage();

protected:
    virtual bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, QWebPage::NavigationType type);
    virtual QString userAgentForUrl(const QUrl& url) const;

Q_SIGNALS:
    void newGame();
    void loadGameFromPGN(const QString &path);
    void loadGameFromFEN(const QString &fen);

private Q_SLOTS:
    void handleUnsupportedContent(QNetworkReply *reply);
    void handleQuery(const QueryItemList &list);
};

#endif
