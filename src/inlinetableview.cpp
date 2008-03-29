#include "inlinetableview.h"

#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QPaintEvent>
#include <QTextCursor>
#include <QTextOption>
#include <QApplication>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

InlineTableView::InlineTableView(QWidget *parent)
    : QAbstractItemView(parent)
{
    m_document = new QTextDocument(this);
    QTextOption option = m_document->defaultTextOption();
    option.setWrapMode(QTextOption::WordWrap);
    m_document->setDefaultTextOption(option);
    setSelectionMode(QAbstractItemView::NoSelection);
}

InlineTableView::~InlineTableView()
{
}

void InlineTableView::setModel(QAbstractItemModel *model)
{
    QAbstractItemView::setModel(model);
    QModelIndex topLeft = model->index(0, 0, rootIndex());
    QModelIndex bottomRight = model->index(model->rowCount() - 1, model->columnCount() - 1, rootIndex());
    dataChanged(topLeft, bottomRight);
}

void InlineTableView::dataChanged(const QModelIndex &topLeft,
                                  const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);

//     qDebug() << "InlineTableView::dataChanged"
//              << "topLeft:" << topLeft.row() << "," << topLeft.column()
//              << "bottomRight:" << bottomRight.row() << "," << bottomRight.column()
//              << endl;

    QTextCursor cursor(m_document);

    QModelIndex index = model()->index(topLeft.row(), topLeft.column(), rootIndex());
    int pos = m_indexToPos.contains(index) ? m_indexToPos.value(index) : -1;
    if (pos == -1) {
//         qDebug() << "moving to end" << endl;
        cursor.movePosition(QTextCursor::End);
    } else {
//         qDebug() << "moving to position" << pos << endl;
        cursor.setPosition(pos);
        cursor.movePosition(QTextCursor::StartOfWord);
    }

    QTextCharFormat bold;
    bold.setFontWeight(QFont::Bold);
    cursor.setCharFormat(bold);

    for (int row = qMax(0, topLeft.row()); row < bottomRight.row() + 1; ++row) {

        QModelIndex rowNumber = model()->index(row, 0, rootIndex());
        if (!m_indexToPos.contains(rowNumber)) {
            QString text = QString("%1. ").arg(QString::number(row + 1));
            cursor.insertText(text);
        }

        for (int column = qMax(0, topLeft.column()); column < bottomRight.column() + 1; ++column) {
            QModelIndex index = model()->index(row, column, rootIndex());
            int pos = m_indexToPos.contains(index) ? m_indexToPos.value(index) : -1;
            QString value = model()->data(index).toString();

            if (pos == -1 && value.isEmpty())
                continue;

            if (pos == -1) {
//                 qDebug() << "new index" << pos << endl;
                m_posToIndex.insert(cursor.position(), index);
                m_indexToPos.insert(index, cursor.position());
            } else {
//                 qDebug() << "previous index at" << pos << endl;
                m_posToIndex.remove(pos);
                m_indexToPos.remove(index);
                QString previousText = m_indexToText.take(index);
                m_posToIndex.insert(cursor.position(), index);
                m_indexToPos.insert(index, cursor.position());
                cursor.clearSelection();
                cursor.movePosition(QTextCursor::NextCharacter, QTextCursor::KeepAnchor, previousText.count());
            }

            QTextCharFormat format;
            QBrush foreground = qVariantValue<QBrush>(model()->data(index, Qt::ForegroundRole));
            QBrush background = qVariantValue<QBrush>(model()->data(index, Qt::BackgroundRole));
//             qDebug() << "foreground" << foreground << "background" << background << endl;

            format.setForeground(foreground);
            format.setBackground(background);
            format.setFontWeight(QFont::Bold);

            QString text = QString("%2 ").arg(value);
            m_indexToText.insert(index, text);
            cursor.setCharFormat(format);
            cursor.insertText(text);
//             cursor.setCharFormat(QTextCharFormat());
        }
    }

    viewport()->update();
}

void InlineTableView::rowsAboutToBeRemoved(const QModelIndex &parent, int start, int end)
{
//     qDebug() << "InlineTableView::rowsAboutToBeRemoved" << endl;
    QAbstractItemView::rowsAboutToBeRemoved(parent, start, end);
}

void InlineTableView::rowsInserted(const QModelIndex &parent, int start, int end)
{
//     qDebug() << "InlineTableView::rowsInserted" << endl;
    QAbstractItemView::rowsInserted(parent, start, end);
    scrollToBottom();
}

QModelIndex InlineTableView::indexAt(const QPoint &point) const
{
    QPoint pt = point + QPoint(horizontalOffset(), verticalOffset());
    int pos = m_document->documentLayout()->hitTest(pt, Qt::ExactHit);
    if (pos == -1)
        return QModelIndex();

    QTextCursor cursor(m_document);
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::StartOfWord);

    if (m_posToIndex.contains(cursor.position())) {
        QModelIndex index = m_posToIndex.value(cursor.position());
        //QString value = model()->data(index).toString();
        //qDebug() << "found index" << cursor.position() << "value" << value << endl;
        return index;
    } else {
        cursor.movePosition(QTextCursor::EndOfWord);
        cursor.movePosition(QTextCursor::NextWord);
        cursor.movePosition(QTextCursor::StartOfWord);

        if (m_posToIndex.contains(cursor.position())) {
            QModelIndex index = m_posToIndex.value(cursor.position());
            //QString value = model()->data(index).toString();
            //qDebug() << "found index at next word" << cursor.position() << "value" << value << endl;
            return index;
        } else {
            //qDebug() << "could not find index" << cursor.position() << endl;
            return QModelIndex();
        }
    }
}

void InlineTableView::scrollTo(const QModelIndex &index, ScrollHint hint)
{
    Q_UNUSED(hint);
    QRect area = viewport()->rect();
    area.moveTop(verticalScrollBar()->value());
    QRect rect = visualRect(index);

//     qDebug() << "scrollTo"
//              << "area" << area
//              << "rect" << rect
//              << endl;

    if (rect.top() < area.top()) { //to the top

        int value = verticalScrollBar()->value() + rect.top() - area.top();
/*        qDebug() << "to the top"
                  << "oldValue" << verticalScrollBar()->value()
                  << "newValue" << value
                  << endl;*/
        verticalScrollBar()->setValue(value);

    } else if (rect.bottom() > area.bottom()) { //to the bottom

        int value = verticalScrollBar()->value() + qMin(
                      rect.bottom() - area.bottom(), rect.top() - area.top());
/*        qDebug() << "to the bottom"
                  << "oldValue" << verticalScrollBar()->value()
                  << "newValue" << value
                  << endl;*/
        verticalScrollBar()->setValue(value);

    }

    update();
/*    switch (hint) {
    case EnsureVisible:
        verticalScrollBar()->setValue(); break;
    case PositionAtTop:
        verticalScrollBar()->setValue(); break;
    case PositionAtBottom:
        verticalScrollBar()->setValue(); break;
    case PositionAtCenter:
        verticalScrollBar()->setValue(); break;
    default:
        break;
    }*/
}

QRect InlineTableView::visualRect(const QModelIndex &index) const
{
    if (!m_indexToPos.contains(index)) {
        return QRect();
    }

    QTextBlock block = m_document->begin();
    QTextLayout *layout = block.layout();
    Q_ASSERT(layout);

    int pos = m_indexToPos.value(index);
    QTextCursor cursor(m_document);
    cursor.setPosition(pos);
    cursor.movePosition(QTextCursor::StartOfWord);

    QTextLine line = layout->lineForTextPosition(cursor.position());
    Q_ASSERT(line.isValid());

//     qDebug() << "line rect" << line.rect() << endl;
    return line.rect().toRect();
}

int InlineTableView::horizontalOffset() const
{
    return horizontalScrollBar()->value();
}

int InlineTableView::verticalOffset() const
{
    return verticalScrollBar()->value();
}

bool InlineTableView::isIndexHidden(const QModelIndex &index) const
{
    Q_UNUSED(index);
    return false;
}

QModelIndex InlineTableView::moveCursor(CursorAction cursorAction, Qt::KeyboardModifiers modifiers)
{
    Q_UNUSED(cursorAction);
    Q_UNUSED(modifiers);
    return QModelIndex();
}

void InlineTableView::setSelection(const QRect &rect, QItemSelectionModel::SelectionFlags flags)
{
    Q_UNUSED(rect);
    Q_UNUSED(flags);
}

QRegion InlineTableView::visualRegionForSelection(const QItemSelection &selection) const
{
    Q_UNUSED(selection);
    return QRegion();
}

void InlineTableView::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    m_document->setTextWidth(viewport()->width());

    QSize viewSize = viewport()->size();
    QSize docSize = m_document->size().toSize();

    verticalScrollBar()->setPageStep(docSize.height());
    horizontalScrollBar()->setPageStep(docSize.width());
    verticalScrollBar()->setRange(0, docSize.height() - viewSize.height());
    horizontalScrollBar()->setRange(0, docSize.width() - viewSize.width());

    QPainter painter(viewport());
    painter.setRenderHint(QPainter::Antialiasing);

    const int xOffset = horizontalOffset();
    const int yOffset = verticalOffset();

    QRect r = event->rect();
    painter.translate(-xOffset, -yOffset);
    r.translate(xOffset, yOffset);

    m_document->drawContents(&painter, r);
}
