#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"

class MainWindow : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public Q_SLOTS:
    void newGame();
    void newGame(const QString &fen);
    void constructGame();
    void loadGameFromPGN();
    void loadGameFromFEN();
    void newScratchBoard();

    void fullScreen(bool show);
    void playButtons(bool show);
    void gameInfo(bool show);

    void offerDraw();
    void resign();
    void convertToScratchBoard();
    void restart();

    void configure();
    void about();

protected:
    virtual void closeEvent(QCloseEvent *event);

private Q_SLOTS:
    void gameStateChanged();
    void tabChanged(int index);
};

#endif
