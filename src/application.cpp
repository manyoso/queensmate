#include "application.h"

#include <QDebug>
#include <QStatusBar>

#include "resource.h"
#include "mainwindow.h"

Application::Application(int &argc, char **argv)
    : QApplication(argc, argv)
{
    QCoreApplication::setOrganizationDomain("castle");
    QCoreApplication::setApplicationName("castle");

    setWindowIcon(QIcon(":icons/application.png"));

    m_resource = new Resource(this);

    m_mainWindow = new MainWindow;
    m_mainWindow->show();
}

Application::~Application()
{
    delete m_mainWindow;
}

QUrl Application::url() const
{
    return QUrl("http://blog.magnuschess.com");
}

void Application::showStatus(const QString &status, int timeout)
{
    mainWindow()->statusBar()->showMessage(status, timeout * 1000);
}
