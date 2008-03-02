#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QWidget>

#include "ui_gameview.h"

#include "chess.h"

class Game;
class Board;
class BoardView;
class Captured;

class GameView : public QWidget, public Ui::GameView {
    Q_OBJECT
public:
    GameView(QWidget *parent, Game *game);
    ~GameView();

    Game *game() const { return m_game; }
    Board *board() const { return m_board; }
    BoardView *boardView() const { return m_boardView; }

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
    void gameStarted();
    void gameEnded();

private:
    Game *m_game;
    Board *m_board;
    BoardView *m_boardView;
    Captured *m_captured;
};

#endif
