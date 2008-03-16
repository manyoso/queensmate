#ifndef WEBVIEW_H
#define WEBVIEW_H

#include <QWebView>
#include <QFrame>

class QLabel;
class QProgressBar;

class ProgressBar :public QFrame {
    Q_OBJECT
public:
    ProgressBar(QWidget *parent);
    ~ProgressBar();

    virtual QSize sizeHint() const;

public Q_SLOTS:
    void startLoad();
    void changeLoad(int change);
    void endLoad();

protected:
    int m_progress;
    QLabel *m_label;
    QProgressBar *m_progressBar;
};

class WebView : public QWebView {
    Q_OBJECT
public:
    WebView(QWidget *parent);
    ~WebView();

    ProgressBar *progressBar() const { return m_progressBar; }

protected:
    virtual void moveEvent(QMoveEvent *event);
    virtual void resizeEvent(QResizeEvent *event);

private:
    void updateProgressGeometry();

private:
    ProgressBar *m_progressBar;
};

#endif
