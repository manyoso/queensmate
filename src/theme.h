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

    static QString pieceThemePath();
    static QString squareThemePath();

    QString piecesTheme() const;
    QString squaresTheme() const;

    QSvgRenderer *rendererForPiece(Chess::Army army, Chess::PieceType piece);
    QBrush brushForSquare(Theme::SquareType squareType);

    QBrush brushForBackground() const;
    QPen penForGrid() const { return m_gridPen; }
    QPen penForLabels() const { return m_labelPen; }
    QPen penForBorder() const { return m_borderPen; }

Q_SIGNALS:
    void themeChanged();

public Q_SLOTS:
    void setPiecesTheme(const QString &theme);
    void setSquaresTheme(const QString &theme);

private:
    void parseSquaresTheme(const QString &theme);
    void saveSquaresTheme(const QString &theme);

private:
    QString m_piecesTheme;
    QString m_squaresTheme;
    QHash<Chess::PieceType, QSvgRenderer*> m_whitePieces;
    QHash<Chess::PieceType, QSvgRenderer*> m_blackPieces;
    QHash<SquareType, QBrush> m_squareBrushes;
    mutable QBrush m_backgroundBrush;
    QPen m_gridPen;
    QPen m_labelPen;
    QPen m_borderPen;
};

#endif
