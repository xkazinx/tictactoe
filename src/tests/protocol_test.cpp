// Headless protocol test: two clients on kazilib's socket log in to a running
// server, one picks the other, X plays the top row and both must get Winner,
// then Play Again from both must start a new game. Exit code = failures.
// Build and run: src\tests\run_protocol_test.cmd (starts and stops the server).
#include <tdef.h>
#include <ctime.h>
#include <chrono>
#include <thread>

static int g_failed = 0;
static void Check(const bool ok, const char * what)
{
	std::cout << (ok ? "ok   " : "FAIL ") << what << "\n";
	if (!ok)
		g_failed++;
}

struct Client
{
	string _name;
	SocketHandler _sock{ Socket::Type::Client, ServerIp(), ServerPort(), 1, 1 };
	vector<Msg> _got;
	string _winner;
	string _auth_name;
	string _left; // OpponentLeft's name
	string _again; // PlayAgain's name (the other asked for a rematch)
	vector<std::pair<string, bool>> _players;
	bool _turn = false;

	void Pump()
	{
		ENetEvent ev;
		for (auto e = _sock.Service(&ev); e != Socket::Event::None; e = _sock.Service(&ev))
		{
			if (e == Socket::Event::Connect)
				OPkt::Create(Msg::Auth, _name)->Send(_sock.Get());
			else if (e == Socket::Event::Packet)
			{
				IPkt in(ev.packet);
				Msg m;
				in.Read(m);
				_got += m;
				if (m == Msg::Winner)
					in.Read(_winner);
				else if (m == Msg::Auth)
					in.Read(_auth_name);
				else if (m == Msg::OpponentLeft)
					in.Read(_left);
				else if (m == Msg::PlayAgain)
					in.Read(_again);
				else if (m == Msg::Players)
				{
					u16 n = 0;
					in.Read(n);
					_players.clear();

					for (int i = 0; i < n; i++)
					{
						string name;
						bool in_game = false;
						in.Read(name, in_game);
						_players.emplace_back(name, in_game);
					}
				}
				else if (m == Msg::StartGame)
				{
					string n1, n2;
					bool t1 = false, t2 = false;
					in.Read(n1, t1, n2, t2);
					_turn = n1 == _name ? t1 : t2;
				}
			}
		}
	}

	bool Got(const Msg m) const { return std::find(_got.begin(), _got.end(), m) != _got.end(); }
	bool InGame(const string & name) const
	{
		for (auto & p : _players)
		{
			if (p.first == name)
				return p.second;
		}
		return false;
	}
	void Send(const OPktPtr & p) { p->Send(_sock.Get()); }
};

// Pumps both until cond holds or 3 s pass
template <typename F>
static bool Until(Client & a, Client & b, F cond)
{
	for (int i = 0; i < 300; i++)
	{
		a.Pump();
		b.Pump();
		if (cond())
			return true;
		std::this_thread::sleep_for(std::chrono::milliseconds(10));
	}
	return false;
}

int main()
{
	Client a, b;
	a._name = "alice";
	b._name = "  bob  "; // spaces at either end are dropped by the server
	a._sock.Connect();
	b._sock.Connect();

	Check(Until(a, b, [&] { return a.Got(Msg::Auth) && b.Got(Msg::Auth); }), "both logged in");
	Check(b._auth_name == "bob", "the server trims the name to \"bob\"");
	Check(Until(a, b, [&] { return a._players.size() == 2 && b._players.size() == 2; }), "both got a list of the two players");

	{
		Client c;
		c._name = "alice";
		c._sock.Connect();
		Check(Until(a, c, [&] { return c.Got(Msg::PlayerExists); }) && !c.Got(Msg::Auth), "a taken name gets PlayerExists");
		c._sock.Disconnect();
	}

	a.Send(OPkt::Create(Msg::SelectPlayer, string("nobody")));
	Check(Until(a, b, [&] { return a.Got(Msg::PlayerGone); }), "picking a player who is not online gets PlayerGone");

	a.Send(OPkt::Create(Msg::SelectPlayer, string("bob")));
	Check(Until(a, b, [&] { return a.Got(Msg::StartGame) && b.Got(Msg::StartGame); }), "game started for both");
	Check(a._turn && !b._turn, "the one who picked plays first");
	Check(Until(a, b, [&] { return a.InGame("bob") && b.InGame("alice"); }), "the list marks both as in a game");

	a.Send(OPkt::Create(Msg::PlayAgain)); // mid-game: ignored
	Until(a, b, [&] { return false; });
	Check(b._again.empty(), "Play Again during a game is ignored");

	// X (alice) takes row 0, O (bob) row 1; out of turn and out of range are ignored
	const int moves[][3] = { { 0, 0, 0 }, { 1, 1, 0 }, { 0, 0, 1 }, { 1, 1, 1 }, { 0, 0, 2 } };

	for (auto & m : moves)
	{
		Client & c = m[0] == 0 ? a : b;
		c.Send(OPkt::Create(Msg::Position, 9, 9));
		c.Send(OPkt::Create(Msg::Position, m[1], m[2]));
		const size_t before = a._got.size();
		Until(a, b, [&] { return a._got.size() > before; });
	}

	Check(Until(a, b, [&] { return a.Got(Msg::Winner) && b.Got(Msg::Winner); }), "both got Winner");
	Check(a._winner == "alice" && b._winner == "alice", "alice won");

	a._got.clear();
	b._got.clear();
	a.Send(OPkt::Create(Msg::PlayAgain));
	Check(Until(a, b, [&] { return b._again == "alice"; }), "bob is told alice wants to play again");
	Check(!a.Got(Msg::StartGame), "one Play Again alone does not restart");
	b.Send(OPkt::Create(Msg::PlayAgain));
	Check(Until(a, b, [&] { return a.Got(Msg::StartGame) && b.Got(Msg::StartGame); }), "Play Again from both starts a new game");
	Check(a._turn && !b._turn, "the one who asked first starts the rematch");

	// A draw: X O X / X O O / O X X
	a._got.clear();
	b._got.clear();
	const int draw[][3] = { { 0, 0, 0 }, { 1, 0, 1 }, { 0, 0, 2 }, { 1, 1, 1 }, { 0, 1, 0 }, { 1, 1, 2 }, { 0, 2, 1 }, { 1, 2, 0 }, { 0, 2, 2 } };

	for (auto & m : draw)
	{
		Client & c = m[0] == 0 ? a : b;
		c.Send(OPkt::Create(Msg::Position, m[1], m[2]));
		const size_t before = a._got.size();
		Until(a, b, [&] { return a._got.size() > before; });
	}

	Check(Until(a, b, [&] { return a.Got(Msg::GameEnded) && b.Got(Msg::GameEnded); }) && !a.Got(Msg::Winner), "a full board with no line is a draw for both");

	a._got.clear();
	b._got.clear();
	a.Send(OPkt::Create(Msg::GoLobby));
	Check(Until(a, b, [&] { return a.Got(Msg::GoLobby) && b.Got(Msg::OpponentLeft); }), "Go to Lobby: alice back in the lobby, bob told she left");
	Check(b._left == "alice", "OpponentLeft names alice");
	Check(Until(a, b, [&] { return !a.InGame("bob") && !b.InGame("alice"); }), "the list shows both free again");

	// A disconnect in the middle of a game
	b._got.clear();
	b._left.clear();
	b.Send(OPkt::Create(Msg::SelectPlayer, string("alice")));
	Check(Until(a, b, [&] { return a.Got(Msg::StartGame) && b.Got(Msg::StartGame); }), "bob starts a game with alice");
	a._sock.Disconnect();
	Check(Until(a, b, [&] { return b._left == "alice"; }), "alice's disconnect: bob told she left");
	Check(Until(a, b, [&] { return b._players.size() == 1; }), "the list drops alice");

	b._sock.Disconnect();
	std::cout << (g_failed ? "FAILED: " : "passed, failed: ") << g_failed << "\n";
	return g_failed;
}
