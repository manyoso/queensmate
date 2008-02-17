#ifndef MOVESMODEL_H
#define MOVESMODEL_H

#include <QStandardItem>
#include <QStandardItemModel>

#include "game.h"
#include "move.h"
#include "chess.h"

class MovesModel;

class MoveItem : public QObject, public QStandardItem {
    Q_OBJECT
public:
    MoveItem();
    ~MoveItem();

    MovesModel *model() const;

    virtual QStandardItem *clone() const;
    virtual QVariant data(int role = Qt::UserRole + 1) const;
    virtual void setData(const QVariant &value, int role = Qt::UserRole + 1);

    void setMove(Move move);

private:
    Move m_move;
};

class MovesModel : public QStandardItemModel {
    Q_OBJECT
public:
    MovesModel(Game *game);
    ~MovesModel();

    Game *game() const { return qobject_cast<Game*>(parent()); }

    void addMove(int fullMoveNumber, Chess::Army army, Move move);

private Q_SLOTS:
    void positionChanged(int oldIndex, int newIndex);
};

#endif
