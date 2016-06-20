#pragma once
class Game;
class Player
{
public:
	Player();
	~Player();

	string _name;
	ENetPeer * _peer;
	bool _in_game, _is_turn, _restart;
	Player * _to;
	string _identifier;
	Game * _game;
	void send(OutPacket * out);
};

