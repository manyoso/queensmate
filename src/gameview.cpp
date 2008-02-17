#include "gameview.h"

#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QSplitter>
#include <QListView>
#include <QBoxLayout>
#include <QPushButton>
#include <QToolButton>
#include <QGroupBox>

#include "game.h"
#include "clock.h"
#include "board.h"
#include "player.h"
#include "captured.h"
#include "boardview.h"
#include "tableview.h"
#include "movesmodel.h"
#include "inlinetableview.h"

using namespace Chess;

int ICON_SIZE = 32;
int PLAYER_SIZE = 32;

GameView::GameView(QWidget *parent, Game *game)
    : QWidget(parent), m_game(game)
{
    setupUi(this);

    m_board = new Board(game);
    m_boardView = new BoardView(ui_boardBox, m_board);

    QHBoxLayout *layout = new QHBoxLayout(ui_boardBox);
    layout->setMargin(0);
    layout->setSpacing(0);
    layout->addWidget(m_boardView);
    ui_boardBox->setLayout(layout);

    ui_flip->setIcon(QIcon(":icons/flip.png"));
    ui_flip->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    connect(ui_flip, SIGNAL(pressed()), m_board, SLOT(flipBoard()));

    ui_begin->setIcon(QIcon(":icons/begin.png"));
    ui_begin->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    connect(ui_begin, SIGNAL(pressed()), this, SLOT(begin()));

    ui_backward->setIcon(QIcon(":icons/backward.png"));
    ui_backward->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    connect(ui_backward, SIGNAL(pressed()), this, SLOT(backward()));

    ui_forward->setIcon(QIcon(":icons/forward.png"));
    ui_forward->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    connect(ui_forward, SIGNAL(pressed()), this, SLOT(forward()));

    ui_end->setIcon(QIcon(":icons/end.png"));
    ui_end->setIconSize(QSize(ICON_SIZE, ICON_SIZE));
    connect(ui_end, SIGNAL(pressed()), this, SLOT(end()));

    ui_whitePlayerIcon->setPixmap(QPixmap(":icons/pieces/wp.png"));
    ui_whitePlayerIcon->setScaledContents(true);
    ui_whitePlayerIcon->setMaximumSize(QSize(PLAYER_SIZE, PLAYER_SIZE));
    ui_blackPlayerIcon->setPixmap(QPixmap(":icons/pieces/bp.png"));
    ui_blackPlayerIcon->setScaledContents(true);
    ui_blackPlayerIcon->setMaximumSize(QSize(PLAYER_SIZE, PLAYER_SIZE));

    if (Player *player = m_game->player(White))
        ui_whitePlayer->setText(player->playerName());
    if (Player *player = m_game->player(Black))
        ui_blackPlayer->setText(player->playerName());

    if (!m_game->clock()->isUnlimited(White))
        ui_whiteClock->setText(m_game->clock()->currentClock(White).toString("mm:ss"));
    else
        ui_whiteClock->setText(tr("Unlimited"));

    if (!m_game->clock()->isUnlimited(Black))
        ui_blackClock->setText(m_game->clock()->currentClock(Black).toString("mm:ss"));
    else
        ui_blackClock->setText(tr("Unlimited"));

    connect(m_game->clock(), SIGNAL(tick()), this, SLOT(tick()));

    QHBoxLayout *piecesLayout = new QHBoxLayout(ui_piecesBox);
    piecesLayout->addWidget(new Captured(ui_piecesBox, m_game));
    ui_piecesBox->setLayout(piecesLayout);

    TableView *table = new TableView(ui_moveTable);
    table->setModel(m_game->moves());

    QHBoxLayout *tableLayout = new QHBoxLayout(ui_moveTable);
    tableLayout->addWidget(table);
    ui_moveTable->setLayout(tableLayout);

    InlineTableView *inlineTable = new InlineTableView(ui_moveInline);
    inlineTable->setModel(m_game->moves());

    QHBoxLayout *inlineLayout = new QHBoxLayout(ui_moveInline);
    inlineLayout->addWidget(inlineTable);
    ui_moveInline->setLayout(inlineLayout);

    ui_infoBox->setVisible(false);
}

GameView::~GameView()
{
}

bool GameView::isPlayButtonsVisible() const
{
    return ui_buttonBox->isVisible();
}

void GameView::setPlayButtonsVisible(bool visible)
{
    ui_buttonBox->setVisible(visible);
}

bool GameView::isGameInfoVisible() const
{
    return ui_rightBox->isVisible();
}

void GameView::setGameInfoVisible(bool visible)
{
    ui_rightBox->setVisible(visible);
}

void GameView::begin()
{
    m_game->setPosition(0);
}

void GameView::backward()
{
    bool atBeginning = m_game->position() == 0;
    if (atBeginning)
        return;

    m_game->setPosition(m_game->position() - 1);
}

void GameView::forward()
{
    bool atEnding = m_game->position() == m_game->count() - 1;
    if (atEnding) {
        return;
    }

    m_game->setPosition(m_game->position() + 1);
}

void GameView::end()
{
    m_game->setPosition(m_game->count() - 1);
}

void GameView::tick()
{
//     qDebug() << "tick" << endl;
    if (!m_game->clock()->isUnlimited(White))
        ui_whiteClock->setText(m_game->clock()->currentClock(White).toString("mm:ss"));
    else
        ui_whiteClock->setText(tr("Unlimited"));

    if (!m_game->clock()->isUnlimited(Black))
        ui_blackClock->setText(m_game->clock()->currentClock(Black).toString("mm:ss"));
    else
        ui_blackClock->setText(tr("Unlimited"));

}
