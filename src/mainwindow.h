#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "ui_mainwindow.h"

class Pgn;
typedef QList<Pgn> PgnList;

class PgnParser;
class DataLoader;
class QProgressBar;

class MainWindow : public QMainWindow, public Ui::MainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();

public Q_SLOTS:
    void newGame();
    void constructGame();
    void loadGameFromPGN();
    void loadGameFromPGN(const QString &path);
    void loadGameFromFEN();
    void loadGameFromFEN(const QString &fen);
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
    void pgnDataProgress(qint64 bytesReceived, qint64 bytesTotal);
    void pgnDataLoaded(const QByteArray &data);
    void pgnDataError(const QString &error);
    void pgnParserProgress(qint64 bytesReceived, qint64 bytesTotal);
    void pgnParserFinished(const PgnList &games);
    void pgnParserError(const QString &error);

private:
    DataLoader *m_pgnLoader;
    PgnParser *m_pgnParser;
    QProgressBar *m_progressBar;
};

#endif
