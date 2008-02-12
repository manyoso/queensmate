#include "tabwidget.h"

#include <QDebug>
#include <QTabBar>

TabWidget::TabWidget(QWidget *parent)
    : QTabWidget(parent)
{
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
