#ifndef THEME_H
#define THEME_H

#include <QWidget>

#include <QPen>
#include <QHash>
#include <QBrush>

#include "chess.h"

class QSvgRenderer;

class Theme : public QObject {
    Q_OBJECT
public:
    enum SquareType { Light, Dark, Attack, Defense, Move };

    Theme(QObject *parent);
    ~Theme();

    QString piecesTheme() const;
    void setPiecesTheme(const QString &theme);

    QString squaresTheme() const;
    void setSquaresTheme(const QString &theme);

    QSvgRenderer *rendererForPiece(Chess::Army army, Chess::PieceType piece);
    QBrush brushForSquare(Theme::SquareType squareType);

    QBrush brushForBackground() const { return m_backgroundBrush; }
    QPen penForGrid() const { return m_gridPen; }
    QPen penForLabels() const { return m_labelPen; }
    QPen penForBorder() const { return m_borderPen; }

private:
    void parseSquaresTheme(const QString &theme);
    void saveSquaresTheme(const QString &theme);

private:
    QString m_piecesTheme;
    QString m_squaresTheme;
    QHash<Chess::PieceType, QSvgRenderer*> m_whitePieces;
    QHash<Chess::PieceType, QSvgRenderer*> m_blackPieces;
    QHash<SquareType, QBrush> m_squareBrushes;
    QBrush m_backgroundBrush;
    QPen m_gridPen;
    QPen m_labelPen;
    QPen m_borderPen;
};

#endif
