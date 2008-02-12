#include "theme.h"

#include <QColor>
#include <QBrush>
#include <QSvgRenderer>

using namespace Chess;

Theme::Theme(QObject *parent)
    : QObject(parent)
{
    QString fileName = QString("themes/default/");

    m_whitePieces.insert(King, new QSvgRenderer(fileName + "wk.svg", this));
    m_whitePieces.insert(Queen, new QSvgRenderer(fileName + "wq.svg", this));
    m_whitePieces.insert(Rook, new QSvgRenderer(fileName + "wr.svg", this));
    m_whitePieces.insert(Bishop, new QSvgRenderer(fileName + "wb.svg", this));
    m_whitePieces.insert(Knight, new QSvgRenderer(fileName + "wn.svg", this));
    m_whitePieces.insert(Pawn, new QSvgRenderer(fileName + "wp.svg", this));

    m_blackPieces.insert(King, new QSvgRenderer(fileName + "bk.svg", this));
    m_blackPieces.insert(Queen, new QSvgRenderer(fileName + "bq.svg", this));
    m_blackPieces.insert(Rook, new QSvgRenderer(fileName + "br.svg", this));
    m_blackPieces.insert(Bishop, new QSvgRenderer(fileName + "bb.svg", this));
    m_blackPieces.insert(Knight, new QSvgRenderer(fileName + "bn.svg", this));
    m_blackPieces.insert(Pawn, new QSvgRenderer(fileName + "bp.svg", this));
}

Theme::~Theme()
{
}

QSvgRenderer *Theme::rendererForPiece(Chess::Army army, Chess::PieceType piece)
{
    if (army == White)
        return m_whitePieces.value(piece);
    else
        return m_blackPieces.value(piece);
}

QBrush Theme::brushForSquare(Theme::SquareType squareType)
{
//     switch (squareType) {
//     case Light:
//         return QBrush(Qt::lightGray);
//     case Dark:
//         return QBrush(Qt::darkGray);
//     case Attack:
//         return QBrush(Qt::darkRed);
//     case Defense:
//         return QBrush(Qt::darkBlue);
//     case Move:
//         return QBrush(Qt::darkGreen);
//     default:
//         return QBrush(Qt::transparent);
//     }
    switch (squareType) {
    case Light:
        return QBrush(QColor(Qt::yellow).lighter());
    case Dark:
        return QBrush(Qt::darkGreen);
    case Attack:
        return QBrush(Qt::darkRed);
    case Defense:
        return QBrush(Qt::darkBlue);
    case Move:
        return QBrush(Qt::black);
    default:
        return QBrush(Qt::transparent);
    }
}
