#ifndef SCRATCHVIEW_H
#define SCRATCHVIEW_H

#include "gameview.h"

class ScratchView : public GameView {
    Q_OBJECT
public:
    ScratchView(QWidget *parent, Game *game);
    ~ScratchView();
};

#endif
