#include "stdafx.h"
#include "Net.h"
#include "App.h"
#include "PlayerMgr.h"
#include "Player.h"
#include "Testerino.h"

void Net::init()
{
	_sock = make_unique<SocketHandler>(Socket::Type::Host, ServerIp(), ServerPort(), 255, 1);
	if (!_sock->Get()->_host)
	{
		const string reason = "could not listen on port " + to_string(ServerPort());
		cout << reason << "\n";
		testerino::Failed(reason);
		std::exit(1);
	}
	cout << "Listening on port " << ServerPort() << "\n";

	const u16 ws_port = (u16)lib::init::Number("ws", ServerPort() + 1);
	if (ws_port)
	{
		std::error_code ec;
		const auto web_dir = lib::exe::Dir() / "web";
		const string web = lib::init::Value("web", std::filesystem::is_directory(web_dir, ec) ? web_dir.string() : string());
		_ws.Start({ .bind = lib::init::Value("ws_bind", "127.0.0.1"), .port = ws_port,
		            .enet_host = "127.0.0.1", .enet_port = ServerPort(), .channels = 1, .max_clients = 255, .web_root = web });
	}
}

void Net::tick()
{
	ENetEvent ev;
	for (auto e = _sock->Service(&ev); e != Socket::Event::None; e = _sock->Service(&ev))
	{
		switch (e)
		{
		case Socket::Event::Connect:
			cout << "Game: Peer Connected\n";
			Socket::ApplyPeerTimeout(ev.peer);
			g_players.add(ev.peer);
			break;

		case Socket::Event::Packet:
		{
			IPkt in(ev.packet); // destroys the enet packet when read
			g_app.msg_process(ev.peer, &in);
			break;
		}

		case Socket::Event::Disconnect:
		{
			cout << "Game: Peer Disconnect\n";
			if (auto p = g_players.find(ev.peer))
			{
				if (p->_in_game || p->_game)
					g_app.not_golobby(p);
			}

			g_players.remove(ev.peer);
			g_app.broadcast_players();
			break;
		}

		default:
			break;
		}
	}
}
