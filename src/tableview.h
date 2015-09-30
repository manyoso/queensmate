#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>

class TableView : public QTableView {
    Q_OBJECT
public:
    TableView(QWidget *parent);
    ~TableView();

    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);
};

#endif
