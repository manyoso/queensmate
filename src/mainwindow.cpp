#include "mainwindow.h"

#include <QDebug>
#include <QSettings>
#include <QBoxLayout>
#include <QCloseEvent>
#include <QFileDialog>
#include <QInputDialog>

#include "pgn.h"
#include "game.h"
#include "clock.h"
#include "board.h"
#include "player.h"
#include "webpage.h"
#include "webview.h"
#include "gameview.h"
#include "notation.h"
#include "resource.h"
#include "pgnparser.h"
#include "boardview.h"
#include "uciengine.h"
#include "dataloader.h"
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
    connect(ui_actionConstructGame, SIGNAL(triggered(bool)), this, SLOT(constructGame()));
    connect(ui_actionLoadGameFromPGN, SIGNAL(triggered(bool)), this, SLOT(loadGameFromPGN()));
    connect(ui_actionLoadGameFromFEN, SIGNAL(triggered(bool)), this, SLOT(loadGameFromFEN()));
    connect(ui_actionNewScratchBoard, SIGNAL(triggered(bool)), this, SLOT(newScratchBoard()));
    connect(ui_actionQuit, SIGNAL(triggered(bool)), chessApp, SLOT(quit()));

    connect(ui_actionFullscreen, SIGNAL(triggered(bool)), this, SLOT(fullScreen(bool)));
    connect(ui_actionPlayButtons, SIGNAL(triggered(bool)), this, SLOT(playButtons(bool)));
    connect(ui_actionGameInfo, SIGNAL(triggered(bool)), this, SLOT(gameInfo(bool)));

    connect(ui_actionOfferDraw, SIGNAL(triggered(bool)), this, SLOT(offerDraw()));
    connect(ui_actionResign, SIGNAL(triggered(bool)), this, SLOT(resign()));
    connect(ui_actionConvertToScratchBoard, SIGNAL(triggered(bool)), this, SLOT(convertToScratchBoard()));
    connect(ui_actionRestart, SIGNAL(triggered(bool)), this, SLOT(restart()));

    connect(ui_actionConfigure, SIGNAL(triggered(bool)), this, SLOT(configure()));
    connect(ui_actionAbout, SIGNAL(triggered(bool)), this, SLOT(about()));

    connect(ui_tabWidget, SIGNAL(currentChanged(int)), this, SLOT(tabChanged(int)));

    m_pgnLoader = new DataLoader(this);
    connect(m_pgnLoader, SIGNAL(progress(qint64, qint64)), this, SLOT(pgnDataProgress(qint64, qint64)));
    connect(m_pgnLoader, SIGNAL(finished(const QByteArray &)), this, SLOT(pgnDataLoaded(const QByteArray &)));
    connect(m_pgnLoader, SIGNAL(error(const QString &)), this, SLOT(pgnDataError(const QString &)));

    m_pgnParser = new PgnParser(this);
    connect(m_pgnParser, SIGNAL(progress(qint64, qint64)), this, SLOT(pgnParserProgress(qint64, qint64)));
    connect(m_pgnParser, SIGNAL(finished(const PgnList &)), this, SLOT(pgnParserFinished(const PgnList &)));
    connect(m_pgnParser, SIGNAL(error(const QString &)), this, SLOT(pgnParserError(const QString &)));

    //begin web...
    m_webView = new WebView(ui_tabWidget);

    QFile about(":/html/about.html");
    Q_ASSERT(about.open(QIODevice::ReadOnly | QIODevice::Text));
    m_aboutPage = new WebPage(this, m_webView);
    m_aboutPage->mainFrame()->setHtml(about.readAll());
    m_webView->setPage(m_aboutPage);

    m_mainPage = new WebPage(this, m_webView);
    connect(m_mainPage->mainFrame(), SIGNAL(initialLayoutCompleted()), this, SLOT(mainPageLayoutCompleted()));
    m_mainPage->mainFrame()->load(chessApp->url());

    connect(m_mainPage->mainFrame(), SIGNAL(loadStarted()),
            m_webView->progressBar(), SLOT(startLoad()));
    connect(m_mainPage, SIGNAL(loadProgress(int)),
            m_webView->progressBar(), SLOT(changeLoad(int)));
    connect(m_mainPage->mainFrame(), SIGNAL(loadFinished()),
            m_webView->progressBar(), SLOT(endLoad()));

    int i = ui_tabWidget->addTab(m_webView, tr("Home"));
    ui_tabWidget->setCurrentIndex(i);

    tabChanged(0);
    //end web...

    ui_toolBar->setVisible(false);

    QStyle *s = QApplication::style();
    QStyleOptionSlider o;
    o.orientation = Qt::Vertical;
    o.state &= ~QStyle::State_Horizontal;
    int extent = s->pixelMetric(QStyle::PM_ScrollBarExtent, &o, 0);
    centralWidget()->setMinimumWidth(1010 /*width of the webpage*/ + (extent * 2) /*width of the scrollbar*/);
    centralWidget()->setMinimumHeight(554);

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
    loadGameFromFEN(QString());
}

void MainWindow::constructGame()
{
    qDebug() << "FIXME!!" << endl;
}

void MainWindow::loadGameFromPGN()
{
    QString file = QFileDialog::getOpenFileName(this, tr("Load PGN File"), QString(), tr("PGN files (*.pgn)"));
    if (file.isEmpty())
        return;

    loadGameFromPGN(file);
}

void MainWindow::loadGameFromPGN(const QString &path)
{
    m_pgnLoader->loadDataFromPath(path);
}

void MainWindow::loadGameFromFEN()
{
    bool ok;
    QString fen = QInputDialog::getText(this, tr("Load FEN String"), tr("FEN String:"), QLineEdit::Normal, QString(), &ok);
    if (!ok)
        return;

    if (fen.isEmpty())
        qDebug() << "ERROR! FEN string is empty" << endl;

    //FIXME error checking...
    loadGameFromFEN(fen);
}

void MainWindow::loadGameFromFEN(const QString &fen)
{
    NewGameDialog dialog(this);
    if (!fen.isEmpty()) {
        dialog.setTypeVisible(false);
        dialog.resize(dialog.minimumSizeHint());
    }

    if (dialog.exec() == QDialog::Rejected)
        return;

    Game *game = 0;
    Player *whitePlayer = 0;
    Player *blackPlayer = 0;

    if (!fen.isEmpty()) {
        game = new Game(this, fen);
    } else if (dialog.isClassicalChess()) {
        game = new Game(this);
    } else {
        game = new Game(this, chessApp->resource()->fenFor960(dialog.isRandom960() ? -1 : dialog.position960()));
        game->setChess960(true);
    }

    connect(game, SIGNAL(gameStarted()), this, SLOT(gameStateChanged()));
    connect(game, SIGNAL(gameEnded()), this, SLOT(gameStateChanged()));

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

    GameView *gameView = new GameView(ui_tabWidget, game);
    gameView->board()->setArmyInFront(!dialog.whiteIsHuman() && dialog.blackIsHuman() ? Black : White);
    game->setParent(gameView); //reparent!!
    game->startGame();

    int i = ui_tabWidget->addTab(gameView,
                                 QString("%1 vs %2").arg(whitePlayer->playerName()).arg(blackPlayer->playerName()));
    ui_tabWidget->setCurrentIndex(i);
}

void MainWindow::newScratchBoard()
{
    Game *game = new Game(this);
    game->setScratchGame(true);

    connect(game, SIGNAL(gameStarted()), this, SLOT(gameStateChanged()));
    connect(game, SIGNAL(gameEnded()), this, SLOT(gameStateChanged()));

    ScratchView *scratchView = new ScratchView(ui_tabWidget, game);
    game->setParent(scratchView); //reparent!!

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

void MainWindow::offerDraw()
{
}

void MainWindow::resign()
{
    GameView *gameView = qobject_cast<GameView*>(ui_tabWidget->currentWidget());
    Q_ASSERT(gameView);
    Player *whitePlayer = gameView->game()->player(White);
    Player *blackPlayer = gameView->game()->player(Black);
    if (whitePlayer->isHuman() && blackPlayer->isHuman()) {
        gameView->game()->endGame(Game::Resignation, gameView->game()->activeArmy() == White ? Game::BlackWins : Game::WhiteWins);
    } else if (whitePlayer->isHuman() && gameView->game()->activeArmy() == White) {
        gameView->game()->endGame(Game::Resignation, Game::BlackWins);
    } else if (blackPlayer->isHuman() && gameView->game()->activeArmy() == Black) {
        gameView->game()->endGame(Game::Resignation, Game::WhiteWins);
    } else if (whitePlayer->isHuman()) {
        gameView->game()->endGame(Game::Resignation, Game::BlackWins);
    } else if (blackPlayer->isHuman()) {
        gameView->game()->endGame(Game::Resignation, Game::WhiteWins);
    }
}

void MainWindow::convertToScratchBoard()
{
    GameView *gameView = qobject_cast<GameView*>(ui_tabWidget->currentWidget());
    Q_ASSERT(gameView);

    Game *game = gameView->game();
    game->setScratchGame(true);

    ScratchView *scratchView = new ScratchView(ui_tabWidget, game);
    game->setParent(scratchView); //reparent!!

    int i = ui_tabWidget->addTab(scratchView, tr("Scratch Board"));
    ui_tabWidget->setCurrentIndex(i);

    delete gameView;
}

void MainWindow::restart()
{
    GameView *gameView = qobject_cast<GameView*>(ui_tabWidget->currentWidget());
    Q_ASSERT(gameView);
    gameView->game()->restartGame();
}

void MainWindow::configure()
{
    ConfigureDialog dialog(this);
    dialog.exec();
}

void MainWindow::about()
{
    AboutDialog dialog(this);
    dialog.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QSettings settings;
    settings.setValue("pos", pos());
    settings.setValue("size", size());

    QMainWindow::closeEvent(event);
}

void MainWindow::gameStateChanged()
{
    tabChanged(ui_tabWidget->currentIndex());
}

void MainWindow::tabChanged(int index)
{
    GameView *gameView = qobject_cast<GameView*>(ui_tabWidget->widget(index));

    ui_actionPlayButtons->setEnabled(gameView != 0 && gameView->game()->ending() != Game::InProgress);
    ui_actionGameInfo->setEnabled(gameView != 0);

    //FIXME can not offer draw or resign when no player is human
    ui_actionOfferDraw->setEnabled(gameView != 0 && gameView->game()->ending() == Game::InProgress);
    ui_actionResign->setEnabled(gameView != 0 && gameView->game()->ending() == Game::InProgress);
    ui_actionConvertToScratchBoard->setEnabled(gameView != 0 && gameView->game()->ending() != Game::InProgress);
    ui_actionRestart->setEnabled(gameView != 0 && gameView->game()->ending() != Game::InProgress);

    ScratchView *scratchView = qobject_cast<ScratchView*>(ui_tabWidget->widget(index));

    ui_actionPlayButtons->setEnabled(scratchView != 0 ? true : ui_actionPlayButtons->isEnabled());
    ui_actionGameInfo->setEnabled(scratchView != 0 ? false : ui_actionGameInfo->isEnabled());

    ui_actionOfferDraw->setEnabled(scratchView != 0 ? false : ui_actionOfferDraw->isEnabled());
    ui_actionResign->setEnabled(scratchView != 0 ? false : ui_actionResign->isEnabled());
    ui_actionConvertToScratchBoard->setEnabled(scratchView != 0 ? false : ui_actionConvertToScratchBoard->isEnabled());
    ui_actionRestart->setEnabled(scratchView != 0 ? true : ui_actionRestart->isEnabled());
}

void MainWindow::mainPageLayoutCompleted()
{
    m_webView->setPage(m_mainPage);
}

void MainWindow::pgnDataProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
}

void MainWindow::pgnDataLoaded(const QByteArray &data)
{
    m_pgnParser->parsePgn(data);
}

void MainWindow::pgnDataError(const QString &error)
{
    qDebug() << "error loading pgn" << error << endl;
}

void MainWindow::pgnParserProgress(qint64 bytesReceived, qint64 bytesTotal)
{
    Q_UNUSED(bytesReceived);
    Q_UNUSED(bytesTotal);
}

void MainWindow::pgnParserFinished(const PgnList &games)
{
    qDebug() << "pgnParserFinished" << endl;
    foreach (Pgn pgn, games) {
        qDebug() << "generating game" << pgn.tag("White") << "VS" << pgn.tag("Black") << endl;
        Game *game = new Game(this);

        Player *whitePlayer = new Player(game);
        whitePlayer->setPlayerName(pgn.tag("White"));

        Player *blackPlayer = new Player(game);
        blackPlayer->setPlayerName(pgn.tag("Black"));

        game->setPlayers(whitePlayer, blackPlayer);

        connect(game, SIGNAL(gameStarted()), this, SLOT(gameStateChanged()));
        connect(game, SIGNAL(gameEnded()), this, SLOT(gameStateChanged()));

        Chess::Army army = White;
        QList<Move> moves = pgn.moves();
        foreach (Move move, moves) {
//             qDebug() << "make move" << Notation::moveToString(move) << endl;
            game->localHumanMadeMove(army, move);
            army = army == White ? Black : White;
        }

        GameView *gameView = new GameView(ui_tabWidget, game);
        game->setParent(gameView); //reparent!!

        int i = ui_tabWidget->addTab(gameView,
                                     QString("%1 vs %2").arg(whitePlayer->playerName()).arg(blackPlayer->playerName()));
        ui_tabWidget->setCurrentIndex(i);
    }
}

void MainWindow::pgnParserError(const QString &error)
{
    qDebug() << "error parsing pgn" << error << endl;
}
