#include "boardview.h"

#include <QDebug>
#include <QResizeEvent>

#include "board.h"
#include "theme.h"

BoardView::BoardView(QWidget *parent, Board *board)
    : QGraphicsView(parent),
    m_board(board)
{
    setFrameStyle(QFrame::NoFrame);
    setCacheMode(QGraphicsView::CacheBackground);
    setBackgroundBrush(m_board->theme()->brushForBackground());
    connect(m_board->theme(), SIGNAL(themeChanged()), this, SLOT(themeChanged()));

    QSizePolicy policy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    policy.setHeightForWidth(true);
    setSizePolicy(policy);

    setScene(board);

    setMinimumSize(400, 400);
}

BoardView::~BoardView()
{
}

void BoardView::themeChanged()
{
    setBackgroundBrush(m_board->theme()->brushForBackground());
    update();
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
