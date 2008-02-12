#ifndef TABWIDGET_H
#define TABWIDGET_H

#include <QTabBar>
#include <QTabWidget>

class QAction;

class TabBar : public QTabBar {
    Q_OBJECT
public:
    TabBar(QWidget *parent);
    ~TabBar();

protected:
    virtual void tabInserted(int index);
    virtual void tabRemoved(int index);

Q_SIGNALS:
    void closeTab(int index);

private Q_SLOTS:
    void closeCurrentTab();

private:
    QAction *m_actionCloseTab;
};

class TabWidget : public QTabWidget {
    Q_OBJECT
public:
    TabWidget(QWidget *parent);
    ~TabWidget();

    bool isTabBarVisible() const;
    void setTabBarVisible(bool visible);

public Q_SLOTS:
    void closeTab(int index);
};

#endif
