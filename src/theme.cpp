#include "theme.h"

#include <QDir>
#include <QDebug>
#include <QSettings>
#include <QSvgRenderer>
#include <QCoreApplication>

using namespace Chess;

Theme::Theme(QObject *parent)
    : QObject(parent)
{
    setObjectName("Theme");

    QSettings settings;
    settings.beginGroup("Themes");
    setPiecesTheme(settings.value("piecesTheme", "default").toString());
    setSquaresTheme(settings.value("squaresTheme", "default").toString());
    settings.endGroup();

//     m_squareBrushes.insert(Light, QBrush(QColor(Qt::yellow).lighter()));
//     m_squareBrushes.insert(Dark, QBrush(Qt::darkGreen));
//     m_squareBrushes.insert(Attack, QBrush(Qt::darkRed));
//     m_squareBrushes.insert(Defense, QBrush(Qt::darkBlue));
//     m_squareBrushes.insert(Move, QBrush(Qt::black));
//     m_backgroundBrush = QBrush(Qt::black);
//     m_gridPen = QPen(Qt::white);
//     m_labelPen = QPen(Qt::lightGray);
//     m_borderPen = QPen(Qt::white);
//     saveSquaresTheme("default.castle");

//     m_squareBrushes.insert(Light, QBrush(Qt::lightGray));
//     m_squareBrushes.insert(Dark, QBrush(Qt::darkGray));
//     m_squareBrushes.insert(Attack, QBrush(Qt::darkRed));
//     m_squareBrushes.insert(Defense, QBrush(Qt::darkBlue));
//     m_squareBrushes.insert(Move, QBrush(Qt::darkGreen));
//     m_backgroundBrush = QBrush(Qt::black);
//     m_gridPen = QPen(Qt::white);
//     m_labelPen = QPen(Qt::lightGray);
//     m_borderPen = QPen(Qt::white);
//     saveSquaresTheme("monocolor.castle");

//     m_squareBrushes.insert(Light, QBrush(Qt::lightGray));
//     m_squareBrushes.insert(Dark, QBrush(Qt::darkRed));
//     m_squareBrushes.insert(Attack, QBrush(Qt::red));
//     m_squareBrushes.insert(Defense, QBrush(Qt::blue));
//     m_squareBrushes.insert(Move, QBrush(Qt::green));
//     m_backgroundBrush = QBrush(Qt::white);
//     m_gridPen = QPen(Qt::black);
//     m_labelPen = QPen(Qt::black);
//     m_borderPen = QPen(Qt::black);
//     saveSquaresTheme("redandgray.castle");

//     m_squareBrushes.insert(Light, QBrush(QColor(209, 205, 184)));
//     m_squareBrushes.insert(Dark, QBrush(QColor(110, 128, 158)));
//     m_squareBrushes.insert(Attack, QBrush(Qt::darkRed));
//     m_squareBrushes.insert(Defense, QBrush(Qt::darkBlue));
//     m_squareBrushes.insert(Move, QBrush(Qt::darkGreen));
//     m_backgroundBrush = QBrush(Qt::white);
//     m_gridPen = QPen(Qt::black);
//     m_labelPen = QPen(Qt::black);
//     m_borderPen = QPen(Qt::black);
//     saveSquaresTheme("monge.castle");
}

Theme::~Theme()
{
}

QString Theme::piecesTheme() const
{
    return m_piecesTheme;
}

void Theme::setPiecesTheme(const QString &theme)
{
    m_piecesTheme = theme;

    qDeleteAll(m_whitePieces);
    qDeleteAll(m_blackPieces);

    QDir appDirectory(QCoreApplication::applicationDirPath());
    appDirectory.cdUp();

    QString fileName = QString(appDirectory.canonicalPath() +
                               QDir::separator() +
                               "themes" +
                               QDir::separator() +
                               "pieces" +
                               QDir::separator() +
                               theme +
                               QDir::separator());

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

QString Theme::squaresTheme() const
{
    return m_squaresTheme;
}

void Theme::setSquaresTheme(const QString &theme)
{
    m_squareBrushes.clear();

    QDir appDirectory(QCoreApplication::applicationDirPath());
    appDirectory.cdUp();

    QString fileName = QString(appDirectory.canonicalPath() +
                               QDir::separator() +
                               "themes" +
                               QDir::separator() +
                               "squares" +
                               QDir::separator() +
                               theme + ".castle");

    parseSquaresTheme(fileName);
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
    if (m_squareBrushes.contains(squareType))
        return m_squareBrushes.value(squareType);

    return QBrush(Qt::transparent);
}

void Theme::parseSquaresTheme(const QString &theme)
{
    QFile file(theme);
    file.open(QIODevice::ReadOnly);
    QDataStream in(&file);

    QBrush light, dark, attack, defense, move;
    in >> light >> dark >> attack >> defense >> move;
    m_squareBrushes.insert(Light, light);
    m_squareBrushes.insert(Dark, dark);
    m_squareBrushes.insert(Attack, attack);
    m_squareBrushes.insert(Defense, defense);
    m_squareBrushes.insert(Move, move);
    in >> m_backgroundBrush;
    in >> m_gridPen;
    in >> m_labelPen;
    in >> m_borderPen;
}

void Theme::saveSquaresTheme(const QString &theme)
{
    QFile file(theme);
    file.open(QIODevice::WriteOnly);
    QDataStream out(&file);
    out << m_squareBrushes.value(Light);
    out << m_squareBrushes.value(Dark);
    out << m_squareBrushes.value(Attack);
    out << m_squareBrushes.value(Defense);
    out << m_squareBrushes.value(Move);
    out << m_backgroundBrush;
    out << m_gridPen;
    out << m_labelPen;
    out << m_borderPen;
}
