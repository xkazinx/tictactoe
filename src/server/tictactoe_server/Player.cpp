#include "stdafx.h"
#include "Player.h"


Player::Player()
{
}


Player::~Player()
{
}

void Player::send(const OPktPtr & out)
{
	out->Send(_peer);
}
