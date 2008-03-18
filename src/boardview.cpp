#include "boardview.h"

#include <QDebug>
#include <QTransform>
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
    QBrush brush = m_board->theme()->brushForBackground();
    brush.setTransform(transform().inverted());
    setBackgroundBrush(brush);
    update();
}

void BoardView::resizeEvent(QResizeEvent *event)
{
    QGraphicsView::resizeEvent(event);
    fitInView(scene()->sceneRect(), Qt::KeepAspectRatio);

    QBrush brush = m_board->theme()->brushForBackground();
    brush.setTransform(transform().inverted());
    setBackgroundBrush(brush);
}

int BoardView::heightForWidth(int w) const
{
    return w;
}
