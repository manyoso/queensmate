#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class MainWindow;
class Resource;

#define chessApp \
  (static_cast<Application*>(QCoreApplication::instance()))

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    Resource *resource() const { return m_resource; }
    MainWindow *mainWindow() const { return m_mainWindow; }

private:
    Resource *m_resource;
    MainWindow *m_mainWindow;
};

#endif
