#include "inlinetableview.h"

#include <QDebug>
#include <QPainter>
#include <QScrollBar>
#include <QTextBlock>
#include <QPaintEvent>
#include <QTextCursor>
#include <QTextOption>
#include <QTextDocument>
#include <QAbstractTextDocumentLayout>

InlineTableView::InlineTableView(QWidget *parent)
    : QAbstractItemView(parent)
{
    m_document = new QTextDocument(this);
    QTextOption option = m_document->defaultTextOption();
    option.setWrapMode(QTextOption::WordWrap);
    m_document->setDefaultTextOption(option);
}

InlineTableView::~InlineTableView()
{
}


void InlineTableView::dataChanged(const QModelIndex &topLeft,
                                  const QModelIndex &bottomRight)
{
    QAbstractItemView::dataChanged(topLeft, bottomRight);

    m_document->clear();

    QTextCursor cursor(m_document);
    cursor.movePosition(QTextCursor::End);

    QTextCharFormat bold;
    bold.setFontWeight(QFont::Bold);

    for (int row = 0; row < model()->rowCount(rootIndex()); ++row) {

        cursor.setCharFormat(bold);
        cursor.insertText(QString("%1. ").arg(QString::number(row + 1)));
        cursor.setCharFormat(QTextCharFormat());

        for (int column = 0; column < model()->columnCount(rootIndex()); ++column) {
            QModelIndex index = model()->index(row, column, rootIndex());
            QString value = model()->data(index).toString();

            if (value.isEmpty())
                continue;

            m_posToIndex.insert(cursor.position(), index);
            m_indexToPos.insert(index, cursor.position());

            QTextCharFormat format;
            QBrush foreground = qVariantValue<QBrush>(model()->data(index, Qt::ForegroundRole));
            QBrush background = qVariantValue<QBrush>(model()->data(index, Qt::BackgroundRole));
            format.setForeground(foreground);
            format.setBackground(background);

            QString display = QString("%2 ").arg(value);
            cursor.setCharFormat(format);
            cursor.insertText(display);
            cursor.setCharFormat(QTextCharFormat());
        }
    }

    viewport()->update();
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
//           qDebug() << "to the top"
//                    << "oldValue" << verticalScrollBar()->value()
//                    << "newValue" << value
//                    << endl;
          verticalScrollBar()->setValue(value);

      } else if (rect.bottom() > area.bottom()) { //to the bottom

          int value = verticalScrollBar()->value() + qMin(
                        rect.bottom() - area.bottom(), rect.top() - area.top());
//           qDebug() << "to the bottom"
//                    << "oldValue" << verticalScrollBar()->value()
//                    << "newValue" << value
//                    << endl;
          verticalScrollBar()->setValue(value);

      }

      update();
//     switch (hint) {
//     case EnsureVisible:
//         verticalScrollBar()->setValue(); break;
//     case PositionAtTop:
//         verticalScrollBar()->setValue(); break;
//     case PositionAtBottom:
//         verticalScrollBar()->setValue(); break;
//     case PositionAtCenter:
//         verticalScrollBar()->setValue(); break;
//     default:
//         break;
//     }
}

QRect InlineTableView::visualRect(const QModelIndex &index) const
{
    if (!m_indexToPos.contains(index))
        return QRect();

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
