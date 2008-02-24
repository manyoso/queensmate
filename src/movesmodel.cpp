#include "movesmodel.h"

#include <QDebug>
#include <QPalette>
#include <QApplication>

#include "notation.h"

using namespace Chess;

MoveItem::MoveItem()
    : QStandardItem()
{
    setEditable(true);
}

MoveItem::~MoveItem()
{
}

MovesModel *MoveItem::model() const
{
    return qobject_cast<MovesModel*>(QStandardItem::model());
}

QStandardItem *MoveItem::clone() const
{
    return 0; //no clones!
}

QVariant MoveItem::data(int role) const
{
    switch (role) {
    case Qt::EditRole:
    case Qt::DisplayRole:
        {
            return (m_move.isValid() ? Notation::moveToString(m_move) : "...");
        }
    case Qt::ForegroundRole:
        {
//             qDebug() << "looking for foreground" << endl;
            int pos = model()->game()->position() - 1;
            int r = pos / 2;
            int c = pos % 2 ? 1 : 0;

//             qDebug() << "pos" << pos
//                      << "r" << r
//                      << "c" << c
//                      << "row" << row()
//                      << "column" << column()
//                      << endl;

            if (pos != -1 && r == row() && c == column()) {
//                 qDebug() << "returning red" << endl;
                return QApplication::palette().link();
            }
        }
    case Qt::DecorationRole:
    case Qt::BackgroundRole:
    case Qt::ToolTipRole:
    case Qt::StatusTipRole:
    case Qt::WhatsThisRole:
    case Qt::SizeHintRole:
    default:
        return QStandardItem::data(role);
    }
}

void MoveItem::setData(const QVariant &value, int role)
{
    if (role == Qt::EditRole) {
        Move move = Notation::stringToMove(value.toString());
        if (!move.isValid())
            return;

        model()->game()->localHumanMadeMove((column() == 0 ? White : Black), move);
    }
}

void MoveItem::setMove(Move move)
{
    m_move = move;
    setEditable(!m_move.isValid());
}

MovesModel::MovesModel(Game *game)
    : QStandardItemModel(game)
{
    QStringList labels;
    labels << tr("White");
    labels << tr("Black");
    setHorizontalHeaderLabels(labels);

    setItemPrototype(new MoveItem); //no clones!!

    //create a placeholder...
    setItem(0, new MoveItem);

    connect(game, SIGNAL(positionChanged(int, int)),
            this, SLOT(positionChanged(int, int)));
}

MovesModel::~MovesModel()
{
}

void MovesModel::addMove(int fullMoveNumber, Chess::Army army, Move move)
{
    MoveItem *moveItem = new MoveItem;
    moveItem->setMove(move);
    setItem(fullMoveNumber - 1, (army == White ? 0 : 1), moveItem);

    //create a placeholder...
    if (army == White)
        setItem(fullMoveNumber - 1, 1, new MoveItem);
    else
        setItem(fullMoveNumber, 0, new MoveItem);
}

void MovesModel::positionChanged(int oldIndex, int newIndex)
{
    {
        int pos = oldIndex - 1;
        int r = pos / 2;
        int c = pos % 2 ? 1 : 0;

        if (pos != -1) {
//             qDebug() << "pos" << pos
//                       << "r" << r
//                       << "c" << c
//                       << endl;

            QStandardItem *i = item(r, c);
            QModelIndex index = indexFromItem(i);
            emit dataChanged(index, index);
        }
    }
    {
        int pos = newIndex - 1;
        int r = pos / 2;
        int c = pos % 2 ? 1 : 0;

        if (pos != -1) {
//             qDebug() << "pos" << pos
//                       << "r" << r
//                       << "c" << c
//                       << endl;

            QStandardItem *i = item(r, c);
            QModelIndex index = indexFromItem(i);
            emit dataChanged(index, index);
        }
    }

}
