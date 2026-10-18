#include "stdafx.h"
#include "App.h"
#include "Net.h"
#include "PlayerMgr.h"
#include "Player.h"
#include "Testerino.h"
#include <thread>
#include <chrono>
#include "Game.h"

void App::init()
{
	g_net.init();
	testerino::Start();
	tick();
	testerino::Stop();
}

void App::tick()
{
	while (_running)
	{
		Clock.seed();
		g_net.tick();
		testerino::Poll();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void App::msg_process(ENetPeer * peer, IPkt * in)
{
	auto p = g_players.find(peer);
	if (!p)
		return;

	Msg msg;
	in->Read(msg);

	// Only Auth before a name is set
	if (msg != Msg::Auth && p->_name.empty())
		return;

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
		not_playagain(p);
		break;

	case Msg::GoLobby:
		not_golobby(p);
		break;

	default:
		break;
	}
}

// Printable characters only (kazilib text/utf8.h), cut at kNameMax bytes on a
// character's boundary, spaces at either end dropped
static string CleanName(const string & raw)
{
	string name = lib::utf8::Printable(raw);
	name.resize(lib::utf8::FitBytes(name, kNameMax));

	const auto b = name.find_first_not_of(' ');
	if (b == string::npos)
		return {};
	return name.substr(b, name.find_last_not_of(' ') - b + 1);
}

void App::not_auth(Player * p, IPkt * in)
{
	string name;
	in->Read(name);
	name = CleanName(name);

	if (name.empty() || !p->_name.empty())
		return;

	if (g_players.find(name))
	{
		p->send(OPkt::Create(Msg::PlayerExists));
		return;
	}

	p->_name = name;
	cout << "Name " << p->_name << " set!\n";

	p->send(OPkt::Create(Msg::Auth, p->_name));
	broadcast_players();
}

void App::update_players(Player * p)
{
	if (p->_name.empty())
		return;

	u16 count = 0;

	for (auto pl : g_players._list)
	{
		if (!pl->_name.empty())
			count++;
	}

	// 1 + 2 + 255 players x (2 + kNameMax 32 + 1): 8928 bytes
	auto out = OPkt::Make(PktCap::Large);
	out->Write(Msg::Players, count);

	for (auto pl : g_players._list)
	{
		if (!pl->_name.empty())
			out->Write(pl->_name, pl->_in_game);
	}

	p->send(out);
}

void App::broadcast_players()
{
	for (auto pl : g_players._list)
		update_players(pl);
}

void App::not_selectplayer(Player * p, IPkt * in)
{
	string name;
	in->Read(name);

	if (p->_in_game)
		return;

	auto to = g_players.find(name);
	if (!to || to == p)
	{
		p->send(OPkt::Create(Msg::PlayerGone));
		return;
	}

	if (to->_in_game)
	{
		p->send(OPkt::Create(Msg::PlayerInGame));
		return;
	}
	cout << p->_name << " requests to play with " << name << "\n";

	start_game(p, to);
	broadcast_players();
}

void App::start_game(Player * p1, Player * p2)
{
	p1->_in_game = true;
	p2->_in_game = true;

	p1->_is_turn = true;
	p2->_is_turn = false;

	p1->_restart = false;
	p2->_restart = false;

	p1->_to = p2;
	p2->_to = p1;

	p1->_identifier = "X";
	p2->_identifier = "O";

	delete p1->_game;
	if (p2->_game != p1->_game)
		delete p2->_game;
	p1->_game = new Game;
	p2->_game = p1->_game;

	auto out = OPkt::Create(Msg::StartGame, p1->_name, p1->_is_turn, p2->_name, p2->_is_turn);
	p1->send(out);
	p2->send(out);
}

void App::not_position(Player * p, IPkt * in)
{
	int x = -1, y = -1;
	in->Read(x, y);

	auto g = p->_game;
	if (!p->_in_game || !g || !p->_to || !p->_is_turn)
		return;

	if (x < 0 || x > 2 || y < 0 || y > 2)
		return;

	if (g->_pos[x][y] != nullptr)
		return;

	// Nothing more once someone has won or the board is full
	if (g->victory() || g->ended())
		return;

	g->_pos[x][y] = p;

	p->_is_turn = false;
	p->_to->_is_turn = true;

	if (auto winner = g->victory())
	{
		p->_is_turn = false;
		p->_to->_is_turn = false;
		update_board(p);

		auto out = OPkt::Create(Msg::Winner, winner->_name);
		p->send(out);
		p->_to->send(out);
	}
	else if (g->ended())
	{
		p->_is_turn = false;
		p->_to->_is_turn = false;
		update_board(p);

		auto out = OPkt::Create(Msg::GameEnded);
		p->send(out);
		p->_to->send(out);
	}
	else
		update_board(p);
}

void App::update_board(Player * p)
{
	// 1 + 2 x (2 + kNameMax 32 + 1) + 9 x (2 + 1): 98 bytes
	auto out = OPkt::Make(PktCap::Small);
	out->Write(Msg::Position, p->_name, p->_is_turn, p->_to->_name, p->_to->_is_turn);

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			const Player * const at = p->_game->_pos[x][y];
			out->Write(at ? at->_identifier : string(" "));
		}
	}

	p->send(out);
	p->_to->send(out);
}

void App::not_playagain(Player * p)
{
	auto p2 = p->_to;
	if (!p2 || !p->_game || p->_restart)
		return;

	// Only once the game is over
	if (!p->_game->victory() && !p->_game->ended())
		return;

	p->_restart = true;
	if (!p2->_restart)
	{
		p2->send(OPkt::Create(Msg::PlayAgain, p->_name));
		return;
	}

	// Both asked: the one who asked first starts, as X
	start_game(p2, p);
}

// p leaves its game (Go to Lobby, or its disconnect): the opponent is told and
// back in the lobby too
void App::not_golobby(Player * p)
{
	if (auto to = p->_to)
	{
		to->_to = nullptr;
		to->_game = nullptr;
		to->_in_game = false;
		to->_restart = false;
		to->send(OPkt::Create(Msg::OpponentLeft, p->_name));
	}

	delete p->_game;
	p->_game = nullptr;
	p->_to = nullptr;
	p->_in_game = false;
	p->_restart = false;

	p->send(OPkt::Create(Msg::GoLobby));
	broadcast_players();
}
