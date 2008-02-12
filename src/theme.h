#ifndef THEME_H
#define THEME_H

#include <QObject>

#include <QHash>

#include "chess.h"

class QColor;
class QBrush;
class QSvgRenderer;

class Theme : public QObject {
    Q_OBJECT
public:
    enum SquareType { Light, Dark, Attack, Defense, Move };

    Theme(QObject *parent);
    ~Theme();

    QSvgRenderer *rendererForPiece(Chess::Army army, Chess::PieceType piece);
    QBrush brushForSquare(Theme::SquareType squareType);

private:
    QHash<Chess::PieceType, QSvgRenderer*> m_whitePieces;
    QHash<Chess::PieceType, QSvgRenderer*> m_blackPieces;
};

#endif
