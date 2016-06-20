#pragma once

class Player;
class App
{
public:
	void init();
	void msg_process(ENetPeer * peer, InPacket * in);
	void not_auth(Player * p, InPacket * in);
	void tick();
	void send(Player * p, OutPacket * out);
	void update_players(Player * p);
	void not_selectplayer(Player * p, InPacket * in);
	void start_game(Player * p1, Player * p2);
	void not_position(Player * p, InPacket * in);
	void update_board(Player * p);

	void not_playagain(Player * p, InPacket * in);
	void not_golobby(Player * p);
};

extern App g_app;