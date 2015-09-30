#include "tabwidget.h"

#include <QDebug>
#include <QAction>

TabBar::TabBar(QWidget *parent)
    : QTabBar(parent)
{
    setContextMenuPolicy(Qt::ActionsContextMenu);

    m_actionCloseTab = new QAction(tr("Close Tab"), this);
    m_actionCloseTab->setVisible(false);
    m_actionCloseTab->setShortcut(Qt::CTRL + Qt::Key_W);
    connect(m_actionCloseTab, SIGNAL(triggered()), this, SLOT(closeCurrentTab()));
    connect(this, SIGNAL(currentChanged(int)), this, SLOT(contextChanged(int)));

    addAction(m_actionCloseTab);
}

TabBar::~TabBar()
{
}

void TabBar::tabInserted(int index)
{
    m_actionCloseTab->setVisible(count() > 1 && index != 0);
    QTabBar::tabInserted(index);
}

void TabBar::tabRemoved(int index)
{
    m_actionCloseTab->setVisible(count() > 1 && index != 0);
    QTabBar::tabRemoved(index);
}

void TabBar::contextChanged(int index)
{
    m_actionCloseTab->setVisible(count() > 1 && index != 0);
}

void TabBar::closeCurrentTab()
{
    emit closeTab(currentIndex());
}

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    TabBar *bar = new TabBar(this);
    connect(bar, SIGNAL(closeTab(int)), this, SLOT(closeTab(int)));
    setTabBar(bar);
}

TabWidget::~TabWidget()
{
}

bool TabWidget::isTabBarVisible() const
{
    return tabBar()->isVisible();
}

void TabWidget::setTabBarVisible(bool visible)
{
    tabBar()->setVisible(visible);
}

void TabWidget::closeTab(int index)
{
    if (QWidget *w = widget(index))
        delete w;
}
