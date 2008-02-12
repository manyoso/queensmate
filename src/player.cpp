#include "player.h"

using namespace Chess;

Player::Player(Game *parent)
    : QObject(parent), m_army(White)
{
}

Player::~Player()
{
}
