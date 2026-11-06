#pragma once

struct Player;
struct App
{
	bool _running = true;

	void init();
	void msg_process(ENetPeer * peer, IPkt * in);
	void not_auth(Player * p, IPkt * in);
	void tick();
	void update_players(Player * p);
	void broadcast_players();
	void not_selectplayer(Player * p, IPkt * in);
	void start_game(Player * p1, Player * p2);
	void not_position(Player * p, IPkt * in);
	void update_board(Player * p);

	void not_playagain(Player * p);
	void not_golobby(Player * p);
};

extern App g_app;
