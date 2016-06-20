#include "stdafx.h"
#include "App.h"
#include "Net.h"
#include "PlayerMgr.h"
#include "Player.h"
#include <thread>
#include "Game.h"

void App::init()
{
	g_net.init();
	tick();
}

void App::tick()
{
	while (true)
	{
		Clock.seed();
		g_net.tick();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void App::msg_process(ENetPeer * peer, InPacket * in)
{
	auto p = g_players.find(peer);
	if (!p)
		return;

	Msg msg;
	in->read(msg);

	switch (msg)
	{
	case Msg::Auth:
		not_auth(p, in);
		break;

	case Msg::SelectPlayer:
		not_selectplayer(p, in);
		break;

	case Msg::Position:
		not_position(p, in);
		break;

	case Msg::PlayAgain:
		not_playagain(p, in);
		break;

	case Msg::GoLobby:
		not_golobby(p);
		break;
	}
}

void App::not_auth(Player * p, InPacket * in)
{
	string name;
	in->read(name);
	auto player = g_players.find(name);
	
	if (player)
	{
		OutPacket out(8);
		out.write(Msg::PlayerExists);
		p->send(&out);
		return;
	}

	p->_name = name;
	cout << "Name " << p->_name << " set!";

	OutPacket out(p->_name.size() + 2 + 8);
	out.write(Msg::Auth, p->_name);
	p->send(&out);

	for (auto pl : g_players._list)
		update_players(pl);
}

void App::send(Player * p, OutPacket * out)
{
	out->send(p->_peer);
}

void App::update_players(Player * p)
{
	int sz = 0;
	for (auto pl : g_players._list)
	{
		sz += p->_name.size() + 2;
	}

	OutPacket out(8 + sz);
	out.write(Msg::Players);

	out.write(u16(g_players._list.size()));
	for (auto pl : g_players._list)
	{
		out.write(pl->_name);
	}
	p->send(&out);
}

void App::not_selectplayer(Player * p, InPacket * in)
{
	string name;
	in->read(name);

	if (p->_in_game)
		return;

	auto to = g_players.find(name);
	if (!to)
		return;

	if (to->_in_game)
	{
		OutPacket out(8);
		out.write(Msg::PlayerInGame);
		p->send(&out);
		return;
	}
	cout << p->_name << " requests to play with " << name << std::endl;

	start_game(p, to);
}

void App::start_game(Player * p1, Player * p2)
{
	p1->_in_game = true;
	p2->_in_game = true;

	p1->_is_turn = true;
	p2->_is_turn = false;

	OutPacket out(p1->_name.size() + 2 + p2->_name.size() + 2 + 16);
	out.write(Msg::StartGame);
	out.write(p1->_name, p1->_is_turn, p2->_name, p2->_is_turn);
	p1->send(&out);
	p1->_to = p2;
	p2->send(&out);
	p2->_to = p1;

	p1->_identifier = "X";
	p2->_identifier = "O";

	p1->_game = new Game;
	p2->_game = p1->_game;
}

void App::not_position(Player * p, InPacket * in)
{
	int x, y;
	in->read(x, y);

	auto g = p->_game;
	if (!p->_in_game)
		return;

	if (!g)
		return;

	if (!p->_to)
		return;

	if (!p->_is_turn)
		return;

	if (g->_pos[x][y] != nullptr)
		return;

	g->_pos[x][y] = p;

	p->_is_turn = false;
	p->_to->_is_turn = true;
	update_board(p);

	if (auto winner = g->victory())
	{
		OutPacket out(winner->_name.size() + 2 + 8);
		out.write(Msg::Winner, winner->_name);
		p->send(&out);
		p->_to->send(&out);
		return;
	}
	else if (g->ended())
	{
		OutPacket out(8);
		out.write(Msg::GameEnded);
		p->send(&out);
		p->_to->send(&out);
	}
}

void App::update_board(Player * p)
{
	OutPacket out(256);
	out.write(Msg::Position);
	out.write(p->_name, p->_is_turn);
	out.write(p->_to->_name, p->_to->_is_turn);
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			if (p->_game->_pos[x][y] != nullptr)
			{
				out.write(p->_game->_pos[x][y]->_identifier);
				cout << "sent " << p->_game->_pos[x][y]->_identifier << std::endl;
			}
			else
			{
				out.write(string(" "));
				cout << "sent " << "empty" << std::endl;

			}
		}
	}
	cout << "sent" << std::endl;
	p->send(&out);
	p->_to->send(&out);
}

void App::not_playagain(Player * p, InPacket * in)
{
	p->_restart = true;

	if (!p->_to)
		return;

	if (p->_to->_restart == false)
		return;

}

void App::not_golobby(Player * p)
{
	OutPacket out(8);
	out.write(Msg::GoLobby);
	
	p->_in_game = false;
	if (p->_game)
	{
		delete p->_game;
		if (p->_to)
		{
			p->_to->_game = nullptr;
			p->_to->send(&out);
			p->_to->_in_game = false;
			p->_to = nullptr;
		}
		p->_game = nullptr;
	}

	p->send(&out);

}