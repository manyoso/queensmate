#include "board.h"

#include <QDebug>
#include <QPainter>
#include <QKeyEvent>
#include <QPixmapCache>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>

#include <math.h>

#include "rules.h"
#include "chess.h"
#include "notation.h"
#include "bitboard.h"
#include "boardsquare.h"
#include "boardpiece.h"

using namespace Chess;

qreal BOARD_SIZE = 20.0;
qreal SQUARE_SIZE = BOARD_SIZE / 8.0;
qreal BORDER_SIZE = SQUARE_SIZE / 2.0;
qreal LETTER_WIDTH = 0.5;
qreal LETTER_HEIGHT = 0.7;

Board::Board(Game *game)
    : QGraphicsScene(game),
      m_currentPiece(0),
      m_armyInFront(White)
{
    m_theme = new Theme(this);

    // generate the board
    int count = 0;
    bool odd = false;
    for (int i = 0; i < 8; ++i) {
        odd = !odd;
        for (int j = 0; j < 8; ++j) {
            BoardSquare *sq = new BoardSquare(this, Square(j, i), QSizeF(SQUARE_SIZE, SQUARE_SIZE), odd);
            m_squares.insert(count++, sq);
            addItem(sq);
            odd = !odd;
        }
    }

    m_borders = new Borders(this);
    addItem(m_borders);

    resetBoard();

    connect(game, SIGNAL(piecesChanged()), this, SLOT(resetBoard()));
}

Board::~Board()
{
}

void Board::setArmyInFront(Chess::Army army)
{
    m_armyInFront = army;
    emit boardFlipped();
}

void Board::flipBoard()
{
    setArmyInFront(armyInFront() == White ? Black : White);
}

void Board::clearBoard()
{
    QPixmapCache::clear();

    // remove the current pieces on the board
    foreach(QGraphicsItem *item, items()) {
        BoardPiece *piece = qgraphicsitem_cast<BoardPiece*>(item);
        if (!piece)
            continue;
        delete piece;
    }

    m_currentPiece = 0;

    clearSquares();
}

void Board::clearSquares()
{
    // reset the squares
    for (int i = 0; i < 64; i++) {
        BoardSquare *sq = m_squares.value(i);
        if (sq->isOdd())
            sq->setSquareType(Theme::Dark);
        else
            sq->setSquareType(Theme::Light);
    }
}

void Board::resetBoard()
{
    clearBoard();

    PieceList whitePieces = game()->pieces(White);
    PieceList::ConstIterator it = whitePieces.begin();
    for (; it != whitePieces.end(); ++it) {
        addItem(new BoardPiece(this, (*it), QSizeF(SQUARE_SIZE, SQUARE_SIZE)));
    }

    PieceList blackPieces = game()->pieces(Black);
    PieceList::ConstIterator it2 = blackPieces.begin();
    for (; it2 != blackPieces.end(); ++it2) {
        addItem(new BoardPiece(this, (*it2), QSizeF(SQUARE_SIZE, SQUARE_SIZE)));
    }
}

void Board::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);

    if (!m_currentPiece)
        return;

    Theme::SquareType type;

    BitBoard b;
    Square sq = m_currentPiece->square();
    if (event->key() == Qt::Key_M) {
        type = Theme::Move;
        b = game()->rules()->bitBoard(sq, Moves);
    } else if (event->key() == Qt::Key_A) {
        type = Theme::Attack;
        if (event->modifiers() & Qt::ControlModifier)
            b = game()->rules()->bitBoard(sq, AttackedBy);
        else
            b = game()->rules()->bitBoard(sq, Attacks);
    } else if (event->key() == Qt::Key_D) {
        type = Theme::Defense;
        if (event->modifiers() & Qt::ControlModifier)
            b = game()->rules()->bitBoard(sq, DefendedBy);
        else
            b = game()->rules()->bitBoard(sq, Defends);
    } else {
        return;
    }

    if (b.isClear())
        return;

    for (int i = 0; i < b.count(); i++) {
        if (b.testBit(i)) {
            int index = i;
            if (armyInFront() == Black) {
                Square s = b.bitToSquare(i);
                Square inverted(7 - s.file(), 7 - s.rank());
                index = inverted.index();
            }
            BoardSquare *sq = m_squares.value(index);
            Q_ASSERT(sq);
            sq->setSquareType(type);
        }
    }
}

void Board::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsScene::keyReleaseEvent(event);
    clearSquares();
}

void Board::hoverEnterSquare()
{
    BoardSquare *square = qobject_cast<BoardSquare*>(sender());
    Q_ASSERT(square);
    m_currentSquare = square->square();
    emit hoverEnter();
}

void Board::hoverLeaveSquare()
{
    m_currentSquare = Square();
    emit hoverLeave();
}

void Board::hoverEnterPiece()
{
    BoardPiece *piece = qobject_cast<BoardPiece*>(sender());
    Q_ASSERT(piece);
    m_currentPiece = piece;
    m_currentSquare = piece->square();
    emit hoverEnter();
}

void Board::hoverLeavePiece()
{
    m_currentPiece = 0;
    m_currentSquare = Square();
    emit hoverLeave();
}

void Board::mouseReleaseEvent(QGraphicsSceneMouseEvent *event)
{
    BoardPiece *piece = qgraphicsitem_cast<BoardPiece*>(mouseGrabberItem());
    if (!piece)
        return;

    QGraphicsScene::mouseReleaseEvent(event);

    if (event->button() != Qt::LeftButton) {
        return;
    }

    int startF = int(ceil(8 - (event->buttonDownScenePos(Qt::LeftButton).x() / SQUARE_SIZE))) - 1;
    int startR = int(ceil(8 - (event->buttonDownScenePos(Qt::LeftButton).y() / SQUARE_SIZE))) - 1;
    int endF = int(ceil(8 - (event->scenePos().x() / SQUARE_SIZE))) - 1;
    int endR = int(ceil(8 - (event->scenePos().y() / SQUARE_SIZE))) - 1;

    startF = armyInFront() == White ? int(ceil((event->buttonDownScenePos(Qt::LeftButton).x() / SQUARE_SIZE))) - 1 : startF;
    startR = armyInFront() == Black ? int(ceil((event->buttonDownScenePos(Qt::LeftButton).y() / SQUARE_SIZE))) - 1 : startR;
    endF = armyInFront() == White ? int(ceil((event->scenePos().x() / SQUARE_SIZE))) - 1 : endF;
    endR = armyInFront() == Black ? int(ceil((event->scenePos().y() / SQUARE_SIZE))) - 1 : endR;

    if ((endF < 0 || endF > 7 || endR < 0 || endR > 7) ||
        (startF == endF && startR == endR)) {
        piece->setSquare(Square(startF, startR));
        return;
    }

    Move move;
    move.setPiece(piece->piece().piece());
    move.setStart(Square(startF, startR));
    move.setEnd(Square(endF, endR));

    if (!game()->localHumanMadeMove(piece->piece().army(), move)) {
        piece->setSquare(Square(startF, startR));
        return;
    }
}


Borders::Borders(Board *board)
    : QGraphicsRectItem(board->sceneRect().adjusted(-BORDER_SIZE, -BORDER_SIZE, BORDER_SIZE, BORDER_SIZE)),
      m_board(board)
{
    setBrush(Qt::lightGray);
    connect(board, SIGNAL(boardFlipped()), this, SLOT(flip()));
    setZValue(1.5);
}

Borders::~Borders()
{
}

void Borders::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPen pen(Qt::white);
    pen.setCosmetic(true);
    painter->setPen(pen);

    QList<QChar> files;
    files << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h';

    QList<QChar> ranks;
    ranks << '1' << '2' << '3' << '4' << '5' << '6' << '7' << '8';

    for (int i = 0; i < 64; i++) {
        BoardSquare *sq = m_board->m_squares.value(i);
        int file = sq->square().file();
        int rank = sq->square().rank();

        QRectF r = sq->mapToScene(sq->boundingRect()).boundingRect();
        painter->drawRect(r);

        if (file == 0) { //first file
            painter->save();
            painter->setPen(Qt::lightGray);

            QChar c = m_board->armyInFront() == White ? ranks.at(rank) : ranks.at(7 - rank);

            QRectF to = r;
            to.setTop(r.top() + ((SQUARE_SIZE - LETTER_HEIGHT) / 2.0));
            to.setLeft(rect().left() + ((BORDER_SIZE - LETTER_WIDTH) / 2.0));
            to.setWidth(LETTER_WIDTH);
            to.setHeight(LETTER_HEIGHT);

            painter->setWorldTransform(textTransform(painter, c, to), true);
            painter->drawText(painter->boundingRect(rect(), Qt::AlignCenter, c), Qt::AlignCenter, c);
            painter->restore();
        }

        if (file == 7) { //last file
            painter->save();
            painter->setPen(Qt::lightGray);

            QChar c = m_board->armyInFront() == White ? ranks.at(rank) : ranks.at(7 - rank);

            QRectF to = r;
            to.setTop(r.top() + ((SQUARE_SIZE - LETTER_HEIGHT) / 2.0));
            to.setLeft(r.right() + ((BORDER_SIZE - LETTER_WIDTH) / 2.0));
            to.setWidth(LETTER_WIDTH);
            to.setHeight(LETTER_HEIGHT);

            painter->setWorldTransform(textTransform(painter, c, to), true);
            painter->drawText(painter->boundingRect(rect(), Qt::AlignCenter, c), Qt::AlignCenter, c);
            painter->restore();
        }

        if (rank == 0) { //first rank
            painter->save();
            painter->setPen(Qt::lightGray);

            QChar c = m_board->armyInFront() == White ? files.at(file) : files.at(7 - file);

            QRectF to = r;
            to.setTop(r.bottom() + ((BORDER_SIZE - LETTER_HEIGHT) / 2.0));
            to.setLeft(r.left() + ((SQUARE_SIZE - LETTER_WIDTH) / 2.0));
            to.setWidth(LETTER_WIDTH);
            to.setHeight(LETTER_HEIGHT);

            painter->setWorldTransform(textTransform(painter, c, to), true);
            painter->drawText(painter->boundingRect(rect(), Qt::AlignCenter, c), Qt::AlignCenter, c);
            painter->restore();
        }

        if (rank == 0) { //last rank
            painter->save();
            painter->setPen(Qt::lightGray);

            QChar c = m_board->armyInFront() == White ? files.at(file) : files.at(7 - file);

            QRectF to = r;
            to.setTop(rect().top() + ((BORDER_SIZE - LETTER_HEIGHT) / 2.0));
            to.setLeft(r.left() + ((SQUARE_SIZE - LETTER_WIDTH) / 2.0));
            to.setWidth(LETTER_WIDTH);
            to.setHeight(LETTER_HEIGHT);

            painter->setWorldTransform(textTransform(painter, c, to), true);
            painter->drawText(painter->boundingRect(rect(), Qt::AlignCenter, c), Qt::AlignCenter, c);
            painter->restore();
        }
    }

    pen.setCosmetic(false);
    pen.setWidthF(0.1);
    painter->setPen(pen);
    painter->setRenderHint(QPainter::Antialiasing, true);
    painter->drawRoundRect(rect());
}

void Borders::flip()
{
    update();
}

QTransform Borders::textTransform(QPainter *painter, const QChar &c, const QRectF &r) const
{
    QRectF from = painter->boundingRect(rect(), Qt::AlignCenter, c);

    QPolygonF from_ = QPolygonF(from);
    from_.pop_back(); //get rid of last closed point

    QPolygonF to_ = QPolygonF(r);
    to_.pop_back(); //get rid of last closed point

    QTransform t;
    QTransform::quadToQuad(from_, to_, t);
    return t;
}
