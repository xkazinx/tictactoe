#include "stdafx.h"
#include "Player.h"


Player::Player()
{
	_in_game = false;
	_to = nullptr;
}


Player::~Player()
{
}

void Player::send(OutPacket * out)
{
	out->send(_peer);
}