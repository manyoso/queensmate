#include "mainwindow.h"

#include <QDebug>
#include <QSettings>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QListView>

#include "game.h"
#include "clock.h"
#include "board.h"
#include "player.h"
#include "gameview.h"
#include "resource.h"
#include "boardview.h"
#include "uciengine.h"
#include "scratchview.h"
#include "application.h"
#include "aboutdialog.h"
#include "newgamedialog.h"
#include "configuredialog.h"

using namespace Chess;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupUi(this);

    connect(ui_actionNewGame, SIGNAL(triggered(bool)), this, SLOT(newGame()));
    connect(ui_actionLoadGame, SIGNAL(triggered(bool)), this, SLOT(loadGame()));
    connect(ui_actionNewScratchBoard, SIGNAL(triggered(bool)), this, SLOT(newScratchBoard()));
    connect(ui_actionQuit, SIGNAL(triggered(bool)), chessApp, SLOT(quit()));

    connect(ui_actionFullscreen, SIGNAL(triggered(bool)), this, SLOT(fullScreen(bool)));
    connect(ui_actionPlayButtons, SIGNAL(triggered(bool)), this, SLOT(playButtons(bool)));
    connect(ui_actionGameInfo, SIGNAL(triggered(bool)), this, SLOT(gameInfo(bool)));

    connect(ui_actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));
    connect(ui_actionConfigure, SIGNAL(triggered(bool)), this, SLOT(configure()));

    connect(ui_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    newScratchBoard();
    tabChanged(0);

    ui_toolBar->setVisible(false);
    ui_statusBar->setVisible(false);

    QSettings settings;
    QPoint pos = settings.value("pos", QPoint(200, 200)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

MainWindow::~MainWindow()
{
}

void MainWindow::newGame()
{
    NewGameDialog dialog(this);
    if (dialog.exec() == QDialog::Rejected)
        return;

    Game *game = 0;
    Player *whitePlayer = 0;
    Player *blackPlayer = 0;

    if (dialog.isClassicalChess()) {
        game = new Game(this);
    } else {
        game = new Game(this, chessApp->resource()->fenFor960(dialog.isRandom960() ? -1 : dialog.position960()));
        game->setChess960(true);
    }

    if (dialog.whiteIsHuman()) {
        whitePlayer = new Player(game);
        whitePlayer->setPlayerName(tr("Human")); //FIXME
    } else {
        QSettings settings;
        settings.beginGroup("Engines");
        QString file = settings.value(dialog.whiteComputer()).toString();
        whitePlayer = new UciEngine(file, game);
        whitePlayer->setPlayerName(dialog.whiteComputer());
        if (!dialog.isClassicalChess())
            qobject_cast<UciEngine*>(whitePlayer)->sendSetOption("UCI_Chess960", true);
    }

    if (dialog.blackIsHuman()) {
        blackPlayer = new Player(game);
        blackPlayer->setPlayerName(tr("Human")); //FIXME
    } else {
        QSettings settings;
        settings.beginGroup("Engines");
        QString file = settings.value(dialog.blackComputer()).toString();
        blackPlayer = new UciEngine(file, game);
        blackPlayer->setPlayerName(dialog.blackComputer());
        if (!dialog.isClassicalChess())
            qobject_cast<UciEngine*>(blackPlayer)->sendSetOption("UCI_Chess960", true);
    }

    if (dialog.whiteClockType() == "Unlimited") {
        game->clock()->setUnlimited(White, true);
    } else {
        QTime base;
        base = base.addSecs(dialog.whiteBase() * 60);
        QTime increment;
        increment = increment.addSecs(dialog.whiteIncrement());
        game->clock()->setBaseTime(White, base);
        game->clock()->setIncrement(White, increment);
        game->clock()->setMoves(White, dialog.whiteMove());
    }

    if (dialog.blackClockType() == "Unlimited") {
        game->clock()->setUnlimited(Black, true);
    } else {
        QTime base;
        base = base.addSecs(dialog.blackBase() * 60);
        QTime increment;
        increment = increment.addSecs(dialog.blackIncrement());
        game->clock()->setBaseTime(Black, base);
        game->clock()->setIncrement(Black, increment);
        game->clock()->setMoves(Black, dialog.blackMove());
    }

    game->setPlayers(whitePlayer, blackPlayer);
    game->startGame();

    GameView *gameView = new GameView(ui_tabWidget, game);
    gameView->board()->setArmyInFront(!dialog.whiteIsHuman() && dialog.blackIsHuman() ? Black : White);

    int i = ui_tabWidget->addTab(gameView,
                                 QString("%1 vs %2").arg(whitePlayer->playerName()).arg(blackPlayer->playerName()));
    ui_tabWidget->setCurrentIndex(i);
}

void MainWindow::loadGame()
{
}

void MainWindow::newScratchBoard()
{
    Game *game = new Game(ui_tabWidget);
    game->setScratchGame(true);
    ScratchView *scratchView = new ScratchView(ui_tabWidget, game);
    int i = ui_tabWidget->addTab(scratchView, tr("Scratch Board"));
    ui_tabWidget->setCurrentIndex(i);
}

void MainWindow::fullScreen(bool show)
{
    if (show) {
        ui_tabWidget->setTabBarVisible(false);

        showFullScreen();
    } else {
        ui_tabWidget->setTabBarVisible(true);

        showNormal();
    }
}

void MainWindow::playButtons(bool show)
{
    GameView *gameView = qobject_cast<GameView*>(ui_tabWidget->currentWidget());
    if (!gameView)
        return;

    gameView->setPlayButtonsVisible(show);
}

void MainWindow::gameInfo(bool show)
{
    GameView *gameView = qobject_cast<GameView*>(ui_tabWidget->currentWidget());
    if (!gameView)
        return;

    gameView->setGameInfoVisible(show);
}

void MainWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::configure()
{
    ConfigureDialog dialog(this);
    dialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());

    QMainWindow::closeEvent(event);
}

void MainWindow::tabChanged(int index)
{
    GameView *gameView = qobject_cast<GameView*>(ui_tabWidget->widget(index));
    ui_actionPlayButtons->setVisible(gameView != 0);
    ui_actionGameInfo->setVisible(gameView != 0);

    ScratchView *scratchView = qobject_cast<ScratchView*>(ui_tabWidget->widget(index));
    ui_actionGameInfo->setVisible(scratchView != 0 ? false : ui_actionGameInfo->isVisible());
}
