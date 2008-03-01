#include "board.h"

#include <QMenu>
#include <QDebug>
#include <QAction>
#include <QPainter>
#include <QKeyEvent>
#include <QPixmapCache>
#include <QStyleOptionGraphicsItem>
#include <QGraphicsSceneMouseEvent>
#include <QGraphicsSceneContextMenuEvent>

#include <math.h>

#include "rules.h"
#include "chess.h"
#include "notation.h"
#include "bitboard.h"
#include "boardpiece.h"
#include "boardsquare.h"
#include "changetheme.h"

using namespace Chess;

qreal BOARD_SIZE = 20.0;
qreal SQUARE_SIZE = BOARD_SIZE / 8.0;
qreal BORDER_SIZE = SQUARE_SIZE / 2.0;
qreal LETTER_WIDTH = 0.4;
qreal LETTER_HEIGHT = 0.7;

Board::Board(Game *game)
    : QGraphicsScene(game),
      m_currentPiece(0),
      m_armyInFront(White),
      m_isMovesShown(false),
      m_isAttacksShown(false),
      m_isDefendsShown(false),
      m_isAttackedByShown(false),
      m_isDefendedByShown(false)
{
    m_theme = new Theme(this);
    connect(m_theme, SIGNAL(themeChanged()), this, SLOT(update()));

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
    connect(this, SIGNAL(currentPieceChanged()), this, SLOT(resetSquares()));
}

Board::~Board()
{
}

void Board::setArmyInFront(Chess::Army army)
{
    m_armyInFront = army;
    emit boardFlipped();
}

QRectF Board::boardRect() const
{
    return sceneRect().adjusted(BORDER_SIZE, BORDER_SIZE, -BORDER_SIZE, -BORDER_SIZE);
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

    if (m_currentPiece != 0) {
        m_currentPiece = 0;
        emit currentPieceChanged();
    }

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

void Board::resetSquares()
{
    clearSquares();

    if (!m_currentPiece)
        return;

    Square sq = m_currentPiece->square();

    QMap<Theme::SquareType, BitBoard> boards;

    if (isMovesShown()) {
        boards.insert(Theme::Move, game()->rules()->bitBoard(sq, Moves));
    }
    if (isAttacksShown()) {
        boards.insert(Theme::Attack, game()->rules()->bitBoard(sq, Attacks));
    }
    if (isDefendsShown()) {
        boards.insert(Theme::Defense, game()->rules()->bitBoard(sq, Defends));
    }
    if (isAttackedByShown()) {
        boards.insert(Theme::Attack, game()->rules()->bitBoard(sq, AttackedBy));
    }
    if (isDefendedByShown()) {
        boards.insert(Theme::Defense, game()->rules()->bitBoard(sq, DefendedBy));
    }

    if (boards.isEmpty())
        return;

    QMap<Theme::SquareType, BitBoard>::ConstIterator it = boards.begin();
    for (; it != boards.end(); ++it) {
        colorBoard(it.key(), it.value());
    }
}

void Board::colorBoard(Theme::SquareType type, const BitBoard &board)
{
    for (int i = 0; i < board.count(); i++) {
        if (board.testBit(i)) {
            int index = i;
            if (armyInFront() == Black) {
                Square s = board.bitToSquare(i);
                Square inverted(7 - s.file(), 7 - s.rank());
                index = inverted.index();
            }
            BoardSquare *sq = m_squares.value(index);
            Q_ASSERT(sq);
            sq->setSquareType(type);
        }
    }
}

void Board::keyPressEvent(QKeyEvent *event)
{
    QGraphicsScene::keyPressEvent(event);

    if (event->key() == Qt::Key_M) {
        setMovesShown(true);
    } else if (event->key() == Qt::Key_A) {
        if (event->modifiers() & Qt::ControlModifier)
            setAttackedByShown(true);
        else
            setAttacksShown(false);
    } else if (event->key() == Qt::Key_D) {
        setDefendsShown(true);
        if (event->modifiers() & Qt::ControlModifier)
            setDefendedByShown(true);
        else
            setDefendsShown(true);
    }
}

void Board::keyReleaseEvent(QKeyEvent *event)
{
    QGraphicsScene::keyReleaseEvent(event);

    if (event->key() == Qt::Key_M) {
        setMovesShown(false);
    } else if (event->key() == Qt::Key_A) {
        if (event->modifiers() & Qt::ControlModifier)
            setAttackedByShown(false);
        else
            setAttacksShown(false);
    } else if (event->key() == Qt::Key_D) {
        setDefendsShown(false);
        if (event->modifiers() & Qt::ControlModifier)
            setDefendedByShown(false);
        else
            setDefendsShown(false);
    }
}

void Board::hoverEnterSquare()
{
    BoardSquare *square = qobject_cast<BoardSquare*>(sender());
    Q_ASSERT(square);
    if (m_currentSquare != square->square()) {
        m_currentSquare = square->square();
        emit currentSquareChanged();
    }
    emit hoverEnter();
}

void Board::hoverLeaveSquare()
{
    if (m_currentSquare != Square()) {
        m_currentSquare = Square();
        emit currentSquareChanged();
    }
    emit hoverLeave();
}

void Board::hoverEnterPiece()
{
    BoardPiece *piece = qobject_cast<BoardPiece*>(sender());
    Q_ASSERT(piece);
    if (m_currentPiece != piece) {
        m_currentPiece = piece;
        emit currentPieceChanged();
    }
    if (m_currentSquare != piece->square()) {
        m_currentSquare = piece->square();
        emit currentSquareChanged();
    }
    emit hoverEnter();
}

void Board::hoverLeavePiece()
{
    if (m_currentPiece != 0) {
        m_currentPiece = 0;
        emit currentPieceChanged();
    }
    if (m_currentSquare != Square()) {
        m_currentSquare = Square();
        emit currentSquareChanged();
    }
    emit hoverLeave();
}

void Board::changeTheme()
{
    ChangeThemeDialog dialog(theme()->piecesTheme(), theme()->squaresTheme(), 0);
    connect(&dialog, SIGNAL(pieceThemeChanged(const QString &)),
            theme(), SLOT(setPiecesTheme(const QString &)));
    connect(&dialog, SIGNAL(squareThemeChanged(const QString &)),
            theme(), SLOT(setSquaresTheme(const QString &)));
    dialog.exec();
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

void Board::contextMenuEvent(QGraphicsSceneContextMenuEvent *event)
{
    QMenu menu;

    QAction *themeAction = menu.addAction(tr("Change Theme..."));
    connect(themeAction, SIGNAL(triggered()), this, SLOT(changeTheme()));

    menu.addSeparator();

    QAction *showMovesAction = menu.addAction(tr("Show Possible Moves"));
    showMovesAction->setCheckable(true);
    showMovesAction->setChecked(isMovesShown());
    showMovesAction->setShortcut(Qt::Key_M);
    connect(showMovesAction, SIGNAL(triggered(bool)), this, SLOT(setMovesShown(bool)));

    QAction *showAttacksAction = menu.addAction(tr("Show Possible Attacks"));
    showAttacksAction->setCheckable(true);
    showAttacksAction->setChecked(isAttacksShown());
    showAttacksAction->setShortcut(Qt::Key_A);
    connect(showAttacksAction, SIGNAL(triggered(bool)), this, SLOT(setAttacksShown(bool)));

    QAction *showDefendsAction = menu.addAction(tr("Show Squares Defended"));
    showDefendsAction->setCheckable(true);
    showDefendsAction->setChecked(isDefendsShown());
    showDefendsAction->setShortcut(Qt::Key_D);
    connect(showDefendsAction, SIGNAL(triggered(bool)), this, SLOT(setDefendsShown(bool)));

    QAction *showAttackedByAction = menu.addAction(tr("Show Attacking Pieces"));
    showAttackedByAction->setCheckable(true);
    showAttackedByAction->setChecked(isAttackedByShown());
    showAttackedByAction->setShortcut(Qt::CTRL + Qt::Key_A);
    connect(showAttackedByAction, SIGNAL(triggered(bool)), this, SLOT(setAttackedByShown(bool)));

    QAction *showDefendedByAction = menu.addAction(tr("Show Defending Pieces"));
    showDefendedByAction->setCheckable(true);
    showDefendedByAction->setChecked(isDefendedByShown());
    showDefendedByAction->setShortcut(Qt::CTRL + Qt::Key_D);
    connect(showDefendedByAction, SIGNAL(triggered(bool)), this, SLOT(setDefendedByShown(bool)));

    menu.exec(event->screenPos());
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

    painter->setPen(m_board->theme()->penForGrid());

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
            painter->setPen(m_board->theme()->penForLabels());

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
            painter->setPen(m_board->theme()->penForLabels());

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
            painter->setPen(m_board->theme()->penForLabels());

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
            painter->setPen(m_board->theme()->penForLabels());

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
