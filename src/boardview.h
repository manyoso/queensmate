#ifndef BOARDVIEW_H
#define BOARDVIEW_H

#include <QGraphicsView>

class Board;

class BoardView : public QGraphicsView {
    Q_OBJECT
public:
    BoardView(QWidget *parent, Board *board);
    ~BoardView();

private Q_SLOTS:
    void themeChanged();

protected:
    virtual void resizeEvent(QResizeEvent *event);
    virtual int heightForWidth(int w) const;

private:
    Board *m_board;
};

#endif
