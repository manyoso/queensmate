#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>

#include "ui_gameview.h"

#include "chess.h"

class Game;
class Board;
class BoardView;

class GameView : public QWidget, public Ui::GameView {
    Q_OBJECT
public:
    GameView(QWidget *parent, Game *game);
    ~GameView();

    Board *board() const { return m_board; }

    bool isPlayButtonsVisible() const;
    void setPlayButtonsVisible(bool visible);

    bool isGameInfoVisible() const;
    void setGameInfoVisible(bool visible);

private Q_SLOTS:
    void begin();
    void backward();
    void forward();
    void end();
    void tick();

private:
    Game *m_game;
    Board *m_board;
    BoardView *m_boardView;
};

#endif
