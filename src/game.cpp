#include "game.h"

#include <QDebug>
#include <QStringList>
#include <QInputDialog>

#include "rules.h"
#include "clock.h"
#include "player.h"
#include "notation.h"
#include "bitboard.h"
#include "resource.h"
#include "movesmodel.h"
#include "mainwindow.h"
#include "application.h"

using namespace Chess;

Game::Game(QObject *parent)
    : QObject(parent),
      m_index(0),
      m_activeArmy(White),
      m_isChess960(false),
      m_isScratchGame(false),
      m_halfMoveClock(0),
      m_fullMoveNumber(1),
      m_fileOfKingsRook(0),
      m_fileOfQueensRook(0)
{
    m_rules = new Rules(this);
    m_clock = new Clock(this);
    m_moves = new MovesModel(this);

    QString fen = QLatin1String("rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1");
    fen = QLatin1String("3R4/2b2pkp/2r3p1/8/1P5P/2P2N2/5PK1/8 w -- - 0 1");

    int oldIndex = m_index;
    m_index = m_mapOfFen.count();
    m_mapOfFen.insert(m_index, fen);
    setFen(fen);
    emit positionChanged(oldIndex, m_index);

    m_rules->refreshBoards();
}

Game::Game(QObject *parent, const QString &fen)
    : QObject(parent),
      m_index(0),
      m_activeArmy(White),
      m_isChess960(false),
      m_isScratchGame(false),
      m_halfMoveClock(0),
      m_fullMoveNumber(1),
      m_fileOfKingsRook(0),
      m_fileOfQueensRook(0)
{
    m_rules = new Rules(this);
    m_clock = new Clock(this);
    m_moves = new MovesModel(this);

    int oldIndex = m_index;
    m_index = m_mapOfFen.count();
    m_mapOfFen.insert(m_index, fen);
    setFen(fen);
    emit positionChanged(oldIndex, m_index);

    m_rules->refreshBoards();
}

Game::~Game()
{
}

int Game::count()
{
    return m_mapOfFen.count();
}

int Game::position() const
{
    return m_index;
}

void Game::setPosition(int index)
{
    if (m_mapOfFen.contains(index)) {
        setFen(m_mapOfFen.value(index));
        int oldIndex = m_index;
        m_index = index;
        emit positionChanged(oldIndex, m_index);
    }
}

QString Game::fen(int index) const
{
    if (m_mapOfFen.contains(index)) {
        return m_mapOfFen.value(index);
    }
    return QString();
}

PieceList Game::pieces(Chess::Army army) const
{
    if (army == White)
        return m_whitePieces.values();
    else
        return m_blackPieces.values();
}

PieceList Game::capturedPieces(Chess::Army army) const
{
    if (army == White)
        return m_whiteCapturedPieces;
    else
        return m_blackCapturedPieces;
}

Player *Game::player(Chess::Army army) const
{
    return (army == White ? m_white : m_black);
}

void Game::setPlayers(Player *white, Player *black)
{
    m_white = white; m_white->setArmy(White);
    m_black = black; m_black->setArmy(Black);
}

bool Game::startGame()
{
    if (m_white->isReady()) {
        m_white->startGame();
        m_white->makeNextMove();
    } else {
        connect(m_white, SIGNAL(ready()), this, SLOT(playerReady()));
    }

    if (m_black->isReady()) {
        m_black->startGame();
    } else {
        connect(m_black, SIGNAL(ready()), this, SLOT(playerReady()));
    }

    connect(m_white, SIGNAL(madeMove(Move)), this, SLOT(playerMadeMove(Move)));
    connect(m_black, SIGNAL(madeMove(Move)), this, SLOT(playerMadeMove(Move)));
    return true;
}

bool Game::endGame()
{
    qDebug() << "FIXME: game ended... now what!" << endl;
    m_clock->endClock();
    disconnect(m_white, SIGNAL(madeMove(Move)), this, SLOT(playerMadeMove(Move)));
    disconnect(m_black, SIGNAL(madeMove(Move)), this, SLOT(playerMadeMove(Move)));
    return false;
}

void Game::playerReady()
{
    Player *player = qobject_cast<Player*>(sender());
    Q_ASSERT(player);
    player->startGame();
    if (player->army() == m_activeArmy) {
        player->makeNextMove();
    }
}

void Game::playerMadeMove(Move move)
{
    Player *player = qobject_cast<Player*>(sender());
    Q_ASSERT(player);

    if (player->isHuman() && !player->isRemote())
        localHumanMadeMove(player->army(), move);
    else
        remoteOrEngineMadeMove(player->army(), move);
}

bool Game::localHumanMadeMove(Chess::Army army, Move move)
{
    if (m_isScratchGame) {
        fillOutMove(army, &move);
        processMove(army, move);
        return true;
    }

    if (m_activeArmy != army)
        return false;

    if (m_activeArmy == White && (!m_white || !m_white->isHuman() || m_white->isRemote()))
        return false;

    if (m_activeArmy == Black && (!m_black || !m_black->isHuman() || m_black->isRemote()))
        return false;

    bool ok = fillOutMove(army, &move);
    if (!ok)
        return false;

    if (!m_rules->isLegalMove(army, move))
        return false;

    processMove(army, move);
    return true;
}

bool Game::remoteOrEngineMadeMove(Chess::Army army, Move move)
{
    if (m_activeArmy != army)
        return false;

    bool ok = fillOutMove(army, &move);
    if (!ok) {
        qDebug() << "ERROR! move is malformed" << endl;
        return false;
    }

    if (!m_rules->isLegalMove(army, move)) {
        qDebug() << "ERROR! attempted illegal move" << Notation::moveToString(move, Chess::Computer) << endl;
        return false;
    }

    processMove(army, move);
    return true;
}

void Game::processMove(Chess::Army army, Move move)
{
    setEnPassantTarget(Square());

    if (army == White) {
        qDebug() << "white moved" << Notation::moveToString(move, Chess::Computer) << endl;

        m_moves->addMove(fullMoveNumber(), army, move);

        if (move.piece() == King) {
            m_rules->setCastleAvailable(White, KingSide, false);
            m_rules->setCastleAvailable(White, QueenSide, false);
        } else if (move.piece() == Rook) {
            if (Notation::squareToString(move.start()) == "a1")
                m_rules->setCastleAvailable(White, QueenSide, false);
            else if (Notation::squareToString(move.start()) == "h1")
                m_rules->setCastleAvailable(White, KingSide, false);
        } else if (move.piece() == Pawn && qAbs(move.start().rank() - move.end().rank()) == 2) {
            setEnPassantTarget(Square(move.end().file(), move.end().rank() - 1));
        }

        m_whiteHistory << move;

        int start = move.start().index();
        int end = move.end().index();

        bool capture = m_blackPieces.contains(end);
        if (capture) {
            Piece piece = m_blackPieces.take(end);
            m_blackCapturedPieces << piece;
            emit capturedPiecesChanged(); //FIXME need to reset state with new fen??
        }

        if (move.piece() != Pawn && !capture) {
            setHalfMoveClock(halfMoveClock() + 1);
        } else {
            setHalfMoveClock(0);
        }

        if (move.isCastle()) { //have to move the rook
            if (move.isKingSideCastle()) {
                Piece rook = m_whitePieces.take(Square(m_fileOfKingsRook, 0).index());
                rook.setSquare(Square(5, 0)); //f1
                m_whitePieces.insert(rook.square().index(), rook);
            } else if (move.isQueenSideCastle()) {
                Piece rook = m_whitePieces.take(Square(m_fileOfQueensRook, 0).index());
                rook.setSquare(Square(3, 0)); //d1
                m_whitePieces.insert(rook.square().index(), rook);
            }
        }

        m_whitePieces.remove(start);
        if (move.promotion() != Unknown)
            m_whitePieces.insert(end, Piece(White, move.promotion(), move.end()));
        else
            m_whitePieces.insert(end, Piece(White, move.piece(), move.end()));
    } else if (army == Black) {
        qDebug() << "black moved" << Notation::moveToString(move, Chess::Computer) << endl;

        m_moves->addMove(fullMoveNumber(), army, move);

        setFullMoveNumber(fullMoveNumber() + 1);

        if (move.piece() == King) {
            m_rules->setCastleAvailable(Black, KingSide, false);
            m_rules->setCastleAvailable(Black, QueenSide, false);
        } else if (move.piece() == Rook) {
            if (Notation::squareToString(move.start()) == "a8")
                m_rules->setCastleAvailable(Black, QueenSide, false);
            else if (Notation::squareToString(move.start()) == "h8")
                m_rules->setCastleAvailable(Black, KingSide, false);
        } else if (move.piece() == Pawn && qAbs(move.start().rank() - move.end().rank()) == 2) {
            setEnPassantTarget(Square(move.end().file(), move.end().rank() + 1));
        }

        m_blackHistory << move;

        int start = move.start().index();
        int end = move.end().index();

        bool capture = m_whitePieces.contains(end);
        if (capture) {
            Piece piece = m_whitePieces.take(end);
            m_whiteCapturedPieces << piece;
            emit capturedPiecesChanged(); //FIXME need to reset state with new fen??
        }

        if (move.piece() != Pawn && !capture) {
            setHalfMoveClock(halfMoveClock() + 1);
        } else {
            setHalfMoveClock(0);
        }

        if (move.isCastle()) { //have to move the rook
            if (move.isKingSideCastle()) {
                Piece rook = m_blackPieces.take(Square(m_fileOfKingsRook, 7).index());
                rook.setSquare(Square(5, 7)); //f8
                m_blackPieces.insert(rook.square().index(), rook);
            } else if (move.isQueenSideCastle()) {
                Piece rook = m_blackPieces.take(Square(m_fileOfQueensRook, 7).index());
                rook.setSquare(Square(3, 7)); //d8
                m_blackPieces.insert(rook.square().index(), rook);
            }
        }

        m_blackPieces.remove(start);
        if (move.promotion() != Unknown)
            m_blackPieces.insert(end, Piece(Black, move.promotion(), move.end()));
        else
            m_blackPieces.insert(end, Piece(Black, move.piece(), move.end()));
    }

    emit pieceMoved();

    m_activeArmy = m_activeArmy == White ? Black : White;

    QString fen = stateOfGameToFen();
    int oldIndex = m_index;
    m_index = m_mapOfFen.count();
    m_mapOfFen.insert(m_index, fen);
    setFen(fen);
    emit positionChanged(oldIndex, m_index);

    m_clock->startClock(m_activeArmy);

    if (halfMoveClock() >= 49) {
        endGame();
    } else if (m_activeArmy == White && m_white) {
        m_white->makeNextMove();
    } else if (m_activeArmy == Black && m_black) {
        m_black->makeNextMove();
    }
}

bool Game::fillOutMove(Chess::Army army, Move *move)
{
    if (!move->isValid())
        return false; //not enough info to do anything

    if (move->piece() == Unknown) {
        int start = move->start().index();
        if (m_whitePieces.contains(start)) {
            PieceType piece = m_whitePieces.value(start).piece();
            move->setPiece(piece);
        } else if (m_blackPieces.contains(start)) {
            PieceType piece = m_blackPieces.value(start).piece();
            move->setPiece(piece);
        }
    }

    if (!move->start().isValid()) {
        bool ok = fillOutStart(army, move);
        if (!ok)
            return false;
    }

    if (move->promotion() == Unknown && move->piece() == Pawn &&
        ((army == White && move->end().rank() == 7) ||
         (army == Black && move->end().rank() == 0))) {

        //FIXME don't allow 'cancel' button...
        QStringList pieces;
        pieces << tr("Queen") << tr("Rook") << tr("Bishop") << tr("Knight");
        QString promotion = QInputDialog::getItem(chessApp->mainWindow(),
                                                  tr("Promote Pawn"),
                                                  tr("Choose Piece:"),
                                                  pieces, 0, false);
        if (promotion == tr("Queen"))
            move->setPromotion(Queen);
        else if (promotion == tr("Rook"))
            move->setPromotion(Rook);
        else if (promotion == tr("Bishop"))
            move->setPromotion(Bishop);
        else if (promotion == tr("Knight"))
            move->setPromotion(Knight);
        else
            move->setPromotion(Queen);
    }

    if (move->piece() == King) {

        //FIXME ...this is different for Chess960...

        if ((move->start().rank() == 0 || move->start().rank() == 7) && move->start().file() == 4) {
            if (move->end().file() == 6) {
                move->setCastle(true);
                move->setCastleSide(KingSide);
            } else if (move->end().file() == 2) {
                move->setCastle(true);
                move->setCastleSide(QueenSide);
            }
        }
    }

    return true;
}

bool Game::fillOutStart(Chess::Army army, Move *move)
{
    if (!move->isValid())
        return false; //not enough info to do anything

    Square square = m_rules->guessSquare(army, *move);
    if (square.isValid()) {
        move->setStart(square);
        return true;
    }

    return false;
}

void Game::setFen(const QString &fen)
{
    m_whitePieces.clear();
    m_blackPieces.clear();

    QStringList list = fen.split(' ');
    Q_ASSERT(list.count() == 6);

    QStringList ranks = list.at(0).split('/');
    Q_ASSERT(ranks.count() == 8);

    for (int i = 0; i < ranks.size(); ++i) {
        QString rank = ranks.at(i);
        int blank = 0;
        for (int j = 0; j < rank.size(); ++j) {
            QChar c = rank.at(j);
            if (c.isLetter() && c.isUpper()) /*white*/ {
                PieceType piece = Notation::charToPiece(c);
                Square square = Square(j + blank, 7 - i);
                m_whitePieces.insert(square.index(), Piece(White, piece, square));
            } else if (c.isLetter() && c.isLower()) /*black*/ {
                PieceType piece = Notation::charToPiece(c.toUpper());
                Square square = Square(j + blank, 7 - i);
                m_blackPieces.insert(square.index(), Piece(Black, piece, square));
            } else if (c.isNumber()) /*blank*/ {
                blank += QString(c).toInt() - 1;
            }
        }
    }

    m_activeArmy = list.at(1) == QLatin1String("w") ? White : Black;

    //Should work for regular fen and UCI fen for chess960...
    QString castling = list.at(2);
    if (castling != "-") {
        QList<QChar> whiteKingSide;
        whiteKingSide << 'K';
        if (isChess960()) whiteKingSide << 'E' << 'F' << 'G' << 'H';
        m_rules->setCastleAvailable(White, KingSide, false);
        foreach (QChar c, whiteKingSide) {
            if (castling.contains(c)) {
                m_rules->setCastleAvailable(White, KingSide, true);
                m_fileOfKingsRook = isChess960() ? castling.indexOf(c) + 3 : 7;
            }
        }

        QList<QChar> whiteQueenSide;
        whiteQueenSide << 'Q';
        if (isChess960()) whiteQueenSide << 'A' << 'B' << 'C' << 'D';
        m_rules->setCastleAvailable(White, QueenSide, false);
        foreach (QChar c, whiteQueenSide) {
            if (castling.contains(c)) {
                m_rules->setCastleAvailable(White, QueenSide, true);
                m_fileOfQueensRook = isChess960() ? castling.indexOf(c) : 0;
            }
        }

        QList<QChar> blackKingSide;
        blackKingSide << 'k';
        if (isChess960()) blackKingSide << 'e' << 'f' << 'g' << 'h';
        m_rules->setCastleAvailable(Black, KingSide, false);
        foreach (QChar c, blackKingSide) {
            if (castling.contains(c)) {
                m_rules->setCastleAvailable(Black, KingSide, true);
                m_fileOfKingsRook = isChess960() ? castling.indexOf(c) + 3 : 7;
            }
        }

        QList<QChar> blackQueenSide;
        blackQueenSide << 'q';
        if (isChess960()) blackQueenSide << 'a' << 'b' << 'c' << 'd';
        m_rules->setCastleAvailable(Black, QueenSide, false);
        foreach (QChar c, blackQueenSide) {
            if (castling.contains(c)) {
                m_rules->setCastleAvailable(Black, QueenSide, true);
                m_fileOfQueensRook = isChess960() ? castling.indexOf(c) : 0;
            }
        }
    }

    QString enPassant = list.at(3);
    if (enPassant != QLatin1String("-")) {
        setEnPassantTarget(Notation::stringToSquare(enPassant));
    }

    setHalfMoveClock(list.at(4).toInt());
    setFullMoveNumber(list.at(5).toInt());

    emit piecesChanged();
}

QString Game::stateOfGameToFen() const
{

    QStringList rankList;
    for (int i = 0; i < 8; ++i) { //rank
        QString rank;
        int blank = 0;
        for (int j = 0; j < 8; ++j) { //file
            Square square(j, 7 - i);
            int index = square.index();
            if (m_whitePieces.contains(index)) {
                Piece piece = m_whitePieces.value(index);
                QChar ch = Notation::pieceToChar(piece.piece());
                if (blank > 0) {
                    rank += QString::number(blank);
                    blank = 0;
                }
                if (!ch.isNull()) {
                    rank += ch.toUpper();
                } else {
                    rank += 'P';
                }
            } else if (m_blackPieces.contains(index)) {
                Piece piece = m_blackPieces.value(index);
                QChar ch = Notation::pieceToChar(piece.piece());
                if (blank > 0) {
                    rank += QString::number(blank);
                    blank = 0;
                }
                if (!ch.isNull()) {
                    rank += ch.toLower();
                } else {
                    rank += 'p';
                }
            } else {
                blank++;
            }
        }

        if (blank > 0) {
            rank += QString::number(blank);
            blank = 0;
        }

        rankList << rank;
    }

    QString ranks = rankList.join("/");
    QString activeArmy = (m_activeArmy == White ? QLatin1String("w") : QLatin1String("b"));

    QList<QChar> files;
    files << 'a' << 'b' << 'c' << 'd' << 'e' << 'f' << 'g' << 'h';

    QString castling;
    if (m_rules->isCastleAvailable(White, KingSide))
        isChess960() ? castling.append(files.at(m_fileOfKingsRook).toUpper()) : castling.append("K");
    if (m_rules->isCastleAvailable(White, QueenSide))
        isChess960() ? castling.append(files.at(m_fileOfQueensRook).toUpper()) : castling.append("Q");
    if (m_rules->isCastleAvailable(Black, KingSide))
        isChess960() ? castling.append(files.at(m_fileOfKingsRook)) : castling.append("k");
    if (m_rules->isCastleAvailable(Black, QueenSide))
        isChess960() ? castling.append(files.at(m_fileOfQueensRook)) : castling.append("q");
    if (castling.isEmpty())
        castling.append("-");

    QString enPassant = enPassantTarget().isValid() ? Notation::squareToString(enPassantTarget()) : QLatin1String("-");

    QStringList fen;
    fen << ranks << activeArmy << castling << enPassant << QString::number(halfMoveClock()) << QString::number(fullMoveNumber());

    return fen.join(" ");
}
