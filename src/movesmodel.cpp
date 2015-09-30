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
            QString result;
            switch (model()->game()->result()) {
            case Game::NoResult: result = QString(); break;
            case Game::WhiteWins: result = "1-0"; break;
            case Game::BlackWins: result = "0-1"; break;
            case Game::Drawn: result = "1/2-1/2"; break;
            default: break;
            }

            if (this != model()->lastMove()) {
                result = QString();
            }

            if (m_move.isValid()) {
                return QString("%1 %2").arg(Notation::moveToString(m_move)).arg(result);
            } else if (model()->game()->result() == Game::NoResult) {
                return QLatin1String("...");
            } else {
                return QString();
            }
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
//                 qDebug() << "returning highlight" << endl;
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

Move MoveItem::move() const
{
    return m_move;
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

    connect(game, SIGNAL(positionChanged(int, int)),
            this, SLOT(positionChanged(int, int)));

    connect(game, SIGNAL(gameStarted()),
            this, SLOT(gameStarted()));

    connect(game, SIGNAL(gameEnded()),
            this, SLOT(gameEnded()));
}

MovesModel::~MovesModel()
{
}

MoveItem *MovesModel::lastMove() const
{
    int pos = game()->position() - 1;
    int r = pos / 2;
    int c = pos % 2 ? 1 : 0;

    if (pos != -1) {
        QStandardItem *i = item(r, c);
        if (i->type() == QStandardItem::UserType + 1) //moveitem
            return static_cast<MoveItem*>(i);
        else
            return 0;
    }
    return 0;
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

void MovesModel::clear(int index)
{
    int pos = index - 1;
    int r = pos / 2;
    int c = pos % 2 ? 1 : 0;
    setRowCount(r + 1);
    if (c == 0) {
        QStandardItem *item = takeItem(r, 1);
        delete item;
    }
    QStandardItem *i = item(r, c);
    QModelIndex current = indexFromItem(i);
    emit dataChanged(current, current);
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

void MovesModel::gameStarted()
{
    //create a placeholder...
    if (game()->activeArmy() == White) {
        setItem(qMax(0, rowCount()), 0, new MoveItem);
    } else {
        setItem(qMax(0, rowCount()), 1, new MoveItem);
    }
}

void MovesModel::gameEnded()
{
    //Remove placeholder for next move...
    QModelIndex topLeft = index(0, 0);
    QModelIndex bottomRight = index(rowCount() - 1, columnCount() - 1);
    if (game()->activeArmy() == White) {
        removeRow(rowCount() - 1);
    }
    emit dataChanged(topLeft, bottomRight);
}
