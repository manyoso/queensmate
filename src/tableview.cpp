#include "tableview.h"

#include <QDebug>

TableView::TableView(QWidget *parent)
    : QTableView(parent)
{
    setSelectionMode(QAbstractItemView::NoSelection);
}

TableView::~TableView()
{
}

void TableView::dataChanged(const QModelIndex &topLeft,
                            const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);

//     qDebug() << "TableView::dataChanged"
//              << "topLeft:" << topLeft.row() << "," << topLeft.column()
//              << "bottomRight:" << bottomRight.row() << "," << bottomRight.column()
//              << endl;
}

void TableView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
//     qDebug() << "TableView::rowsAboutToBeRemoved" << endl;
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}

void TableView::rowsInserted(const QModelIndex &parent, int start, int end)
{
//    qDebug() << "TableView::rowsInserted" << endl;
    QAbstractItemView::rowsInserted(parent, start, end);
    scrollToBottom();
}
