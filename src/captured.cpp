#include "captured.h"

#include <QDebug>
#include <QLabel>
#include <QBoxLayout>

#include "game.h"
#include "chess.h"
#include "piece.h"

using namespace Chess;

int PIECE_SIZE = 22;
int PIECE_MARGIN = 2;

Captured::Captured(QWidget *parent, Game *game)
    : QWidget(parent),
      m_game(game)
{
    m_whiteBox = new QWidget(this);
    m_blackBox = new QWidget(this);

    m_whiteLayout = new QHBoxLayout(m_whiteBox);
    m_whiteLayout->setMargin(0);
    m_whiteLayout->setSpacing(0);
    m_whiteLayout->addStretch(1);
    m_whiteBox->setLayout(m_whiteLayout);

    m_blackLayout = new QHBoxLayout(m_blackBox);
    m_blackLayout->setMargin(0);
    m_blackLayout->setSpacing(0);
    m_blackLayout->addStretch(1);
    m_blackBox->setLayout(m_blackLayout);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_whiteBox);
    layout->addWidget(m_blackBox);
    setLayout(layout);

    resetPieces();

    connect(game, SIGNAL(capturedPiecesChanged()), this, SLOT(resetPieces()));

    setMinimumHeight(PIECE_SIZE);
    setMinimumWidth(PIECE_SIZE * 15);
}

Captured::~Captured()
{
}

void Captured::resetPieces()
{
    if (!m_game)
        return;

    // remove the current labels
    QList<QLabel*> whiteLabels = m_whiteBox->findChildren<QLabel*>();
    foreach(QLabel *label, whiteLabels) {
        delete label;
    }

    QList<QLabel*> blackLabels = m_blackBox->findChildren<QLabel*>();
    foreach(QLabel *label, blackLabels) {
        delete label;
    }

    QLayoutItem *whiteSpacer = m_whiteLayout->takeAt(m_whiteLayout->count() - 1);
    if (whiteSpacer && whiteSpacer->spacerItem())
        delete whiteSpacer;

    QLayoutItem *blackSpacer = m_blackLayout->takeAt(m_blackLayout->count() - 1);
    if (blackSpacer && blackSpacer->spacerItem())
        delete blackSpacer;

    PieceList whitePieces = m_game->capturedPieces(White);
    PieceList::ConstIterator it = whitePieces.begin();
    for (; it != whitePieces.end(); ++it) {
        QLabel *label = new QLabel(m_whiteBox);
        label->setMargin(PIECE_MARGIN);
        label->setMaximumSize(QSize(PIECE_SIZE, PIECE_SIZE));
        label->setScaledContents(true);
        label->setPixmap(pixmapForPiece((*it)));
        m_whiteLayout->addWidget(label);
    }

    PieceList blackPieces = m_game->capturedPieces(Black);
    PieceList::ConstIterator it2 = blackPieces.begin();
    for (; it2 != blackPieces.end(); ++it2) {
        QLabel *label = new QLabel(m_blackBox);
        label->setMargin(PIECE_MARGIN);
        label->setMaximumSize(QSize(PIECE_SIZE, PIECE_SIZE));
        label->setScaledContents(true);
        label->setPixmap(pixmapForPiece((*it2)));
        m_blackLayout->addWidget(label);
    }

    if (whitePieces.isEmpty()) {
        QLabel *label = new QLabel(m_whiteBox);
        label->setMargin(PIECE_MARGIN);
        label->setFixedSize(QSize(PIECE_SIZE, PIECE_SIZE));
        m_whiteLayout->addWidget(label); //dummy
    }

    if (blackPieces.isEmpty()) {
        QLabel *label = new QLabel(m_blackBox);
        label->setMargin(PIECE_MARGIN);
        label->setFixedSize(QSize(PIECE_SIZE, PIECE_SIZE));
        m_blackLayout->addWidget(label); //dummy
    }

    m_whiteLayout->addStretch(1);
    m_blackLayout->addStretch(1);
}

QPixmap Captured::pixmapForPiece(Piece piece) const
{
    QString fileName = piece.army() == White ? "w" : "b";
    switch (piece.piece()) {
    case King:
        fileName += "k.png"; break;
    case Queen:
        fileName += "q.png"; break;
    case Rook:
        fileName += "r.png"; break;
    case Bishop:
        fileName += "b.png"; break;
    case Knight:
        fileName += "n.png"; break;
    case Pawn:
        fileName += "p.png"; break;
    default:
        break;
    }

    return QPixmap(QString(":icons/pieces/%1").arg(fileName));
}
