#include "boardview.h"

#include <QDebug>
#include <QResizeEvent>

#include "board.h"
#include "theme.h"

BoardView::BoardView(QWidget *parent, Board *board)
    : QGraphicsView(parent)
{
    setFrameStyle(QFrame::NoFrame);
    setCacheMode(QGraphicsView::CacheBackground);
    setBackgroundBrush(board->theme()->brushForBackground());

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);

    setScene(board);

    setMinimumSize(400, 400);
}

BoardView::~BoardView()
{
}

void BoardView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene()->sceneRect().adjusted(-0.5, -0.5, 1.0, 1.0), Qt::KeepAspectRatio);
}

int BoardView::heightForWidth(int w) const
{
    return w;
}
