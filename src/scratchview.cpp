#include "scratchview.h"

#include <QDebug>

using namespace Chess;

ScratchView::ScratchView(QWidget *parent, Game *game)
    : GameView(parent, game)
{
    ui_rightBox->setVisible(false);
}

ScratchView::~ScratchView()
{
}
