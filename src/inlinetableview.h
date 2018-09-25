#ifndef INLINETABLEVIEW_H
#define INLINETABLEVIEW_H

#include <QAbstractItemView>

#include <QHash>

class QPaintEvent;
class QModelIndex;
class QTextDocument;

class InlineTableView : public QAbstractItemView {
    Q_OBJECT
public:
    InlineTableView(QWidget *parent);
    ~InlineTableView();

    //inherited from QAbstractItemView
    virtual void setModel(QAbstractItemModel *model);
    virtual QModelIndex indexAt(const QPoint &point) const;
    virtual void scrollTo(const QModelIndex &index, ScrollHint hint = EnsureVisible);
    virtual QRect visualRect(const QModelIndex &index) const;

protected Q_SLOTS:
    virtual void dataChanged(const QModelIndex &topLeft,
                             const QModelIndex &bottomRight);
    virtual void rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end);
    virtual void rowsInserted(const QModelIndex &parent, int start, int end);

protected:
    //inherited from QAbstractItemView
    virtual int horizontalOffset() const;
    virtual int verticalOffset() const;
    virtual bool isIndexHidden(const QModelIndex &index) const;
    virtual QModelIndex moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers);
    virtual void setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags);
    virtual QRegion visualRegionForSelection(const QItemSelection &selection) const;

    //inherited from QWidget
    virtual void paintEvent(QPaintEvent *event);

private:
    QTextDocument *m_document;
    QHash<QModelIndex, int> m_indexToPos;
    QHash<int, QModelIndex> m_posToIndex;

    QHash<QModelIndex, int> m_rowToPos;
    QHash<int, QModelIndex> m_posToRow;

    QHash<QModelIndex, QString> m_indexToText;
    QHash<QModelIndex, QString> m_rowToText;
};

#endif
