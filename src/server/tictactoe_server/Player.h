#pragma once
class Game;
class Player
{
public:
	Player();
	~Player();

	string _name;
	ENetPeer * _peer = nullptr;
	bool _in_game = false, _is_turn = false, _restart = false;
	Player * _to = nullptr;
	string _identifier;
	Game * _game = nullptr;
	void send(const OPktPtr & out);
};
