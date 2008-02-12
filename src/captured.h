#ifndef CAPTURED_H
#define CAPTURED_H

#include <QWidget>
#include <QPointer>

class Game;
class Piece;
class QPixmap;
class QHBoxLayout;

class Captured : public QWidget {
    Q_OBJECT
public:
    Captured(QWidget *parent, Game *game);
    ~Captured();

private Q_SLOTS:
    void resetPieces();

private:
    QPixmap pixmapForPiece(Piece piece) const;

private:
    QWidget *m_whiteBox;
    QWidget *m_blackBox;
    QHBoxLayout *m_whiteLayout;
    QHBoxLayout *m_blackLayout;
    QPointer<Game> m_game;
};

#endif
