#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>
#include <QUrl>

class Resource;
class MainWindow;
class QNetworkReply;

#define chessApp \
  (static_cast<Application*>(QCoreApplication::instance()))

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    QUrl url() const;

    Resource *resource() const { return m_resource; }
    MainWindow *mainWindow() const { return m_mainWindow; }

public Q_SLOTS:
    void showStatus(const QString &status, int timeout = 2 /*secs*/);

private:
    Resource *m_resource;
    MainWindow *m_mainWindow;
};

#endif
