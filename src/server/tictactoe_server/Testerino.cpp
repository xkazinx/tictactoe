#include "stdafx.h"
#include "Testerino.h"
#include "App.h"
#include "Game.h"
#include "Net.h"
#include "Player.h"
#include "PlayerMgr.h"
#ifdef _WIN32
#include <testerino/tserver.h>

namespace testerino
{
	namespace
	{
		tcore::ServerBridge g_bridge;

		tcore::JObj PlayerJson(const Player * const p)
		{
			tcore::JObj o;
			o.Str("name", p->_name).Bool("in_game", p->_in_game).Bool("turn", p->_is_turn).Bool("restart", p->_restart);
			o.Str("mark", p->_identifier).Str("opponent", p->_to ? p->_to->_name : string());
			return o;
		}

		Player * FindNamed(const string & name)
		{
			return name.empty() ? nullptr : g_players.find(name);
		}

		void AddVerbs(tcore::ServerBridge & b)
		{
			b.Verb("players", "players", "every connected peer: name (empty before Auth), in_game, turn, mark, opponent", [](const tcore::Cmd &, tcore::Job & j) {
				tcore::JArr a;

				for (auto p : g_players._list)
					a.Obj(PlayerJson(p));

				j.Ok(tcore::JObj().Int("count", (long long)g_players._list.size()).Raw("players", a.Done()));
			});

			b.Verb("board", "board <name>", "the board of <name>'s game: rows of X / O / . (row x, column y)", [](const tcore::Cmd & c, tcore::Job & j) {
				auto p = FindNamed(c.Arg(0));
				if (!p)
					return j.Fail("no player named " + c.Arg(0));
				if (!p->_game)
					return j.Fail(p->_name + " is not in a game");

				tcore::JArr rows;

				for (int x = 0; x < 3; x++)
				{
					string row;

					for (int y = 0; y < 3; y++)
					{
						const Player * const at = p->_game->_pos[x][y];
						row += at ? at->_identifier : ".";
					}

					rows.Str(row);
				}

				const Player * const winner = p->_game->victory();
				j.Ok(tcore::JObj().Raw("rows", rows.Done()).Str("winner", winner ? winner->_name : string()).Bool("full", p->_game->ended()));
			});

			b.Verb("kick", "kick <name>", "disconnects that player (a lost connection as its opponent sees it)", [](const tcore::Cmd & c, tcore::Job & j) {
				auto p = FindNamed(c.Arg(0));
				if (!p)
					return j.Fail("no player named " + c.Arg(0));
				// The disconnect event runs the usual cleanup on the next service
				enet_peer_disconnect(p->_peer, 0);
				j.Ok(tcore::JObj().Str("kicked", p->_name));
			});

			b.Verb("quit", "quit", "ends the server", [](const tcore::Cmd &, tcore::Job & j) {
				g_app._running = false;
				j.Ok();
			});

			b.Run().AddCond("players", "players=<n> / players>=<n>: named players connected", [](const string & v, const char op, tcore::Cond & out, string & err) {
				long long want = 0;
				if (!tcore::ToInt(v, want))
				{
					err = "players=<number>";
					return false;
				}
				out = [want, op](string & d) {
					long long n = 0;

					for (auto p : g_players._list)
					{
						if (!p->_name.empty())
							n++;
					}

					d = to_string(n);
					return op == '=' ? n == want : n >= want;
				};
				return true;
			});

			b.Run().AddCond("in_game", "in_game=<name>: that player is in a game", [](const string & v, char, tcore::Cond & out, string &) {
				out = [v](string & d) {
					auto p = FindNamed(v);
					d = p ? (p->_in_game ? "in game" : "lobby") : "absent";
					return p && p->_in_game;
				};
				return true;
			});

			b.Run().ping_fields = [](tcore::JObj & o) { o.Int("port", ServerPort()); };
		}
	}

	void Prepare()
	{
		tcore::ServerBridge::Prepare();
	}

	void Start()
	{
		std::error_code ec;
		const bool dev = tcore::fs::exists(tcore::CheckoutFromCwd() / "src" / "tictactoe.sln", ec);
		g_bridge.Start(lib::init::Flag("testerino", false), dev, "(!) testerino is only on in a source checkout (src\\tictactoe.sln)", AddVerbs);
	}

	void Poll()
	{
		g_bridge.Poll();
	}

	void Stop()
	{
		g_bridge.Stop();
	}

	void Failed(const string & reason)
	{
		if (lib::init::Flag("testerino", false))
			tcore::WriteFailed(tcore::CheckoutFromCwd() / "testerino", "server", reason, { reason });
	}
}
#else
// Linux (the deployed server): no bridge, the tests drive the Windows build
namespace testerino
{
	void Prepare() {}
	void Start() {}
	void Poll() {}
	void Stop() {}
	void Failed(const string &) {}
}
#endif
