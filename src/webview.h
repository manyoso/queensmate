#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>

class WebView : public QWebView {
  Q_OBJECT
public:
    WebView(QWidget *parent);
    ~WebView();
};

#endif
