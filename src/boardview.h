#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QGraphicsView>

class Board;

class BoardView : public QGraphicsView {
    Q_OBJECT
public:
    BoardView(QWidget *parent, Board *board);
    ~BoardView();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual int heightForWidth(int w) const;
};

#endif
