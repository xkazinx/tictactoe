#include "stdafx.h"
#include "Net.h"
#include "App.h"
#include "PlayerMgr.h"
#include "Player.h"

void Net::init()
{
	_sock.create(Socket::Type::Host, _IP, _PORT, 255, 1);
}

void Net::tick()
{
	ENetEvent ev;
	while (_sock.get()->get(&ev) > 0) {
		switch (ev.type) {
		case Socket::Event::Connect:
		{
			cout << "Game: Peer Connected\n";
			ev.peer->data = nullptr;
			g_players.add(ev.peer);
		}
		break;

		case Socket::Event::Packet:
		{
			g_app.msg_process(ev.peer, new InPacket(ev.packet));
			break;
		}

		case Socket::Event::Disconnect:
		{
			cout << "Game: Peer Disconnect\n";
			auto p = g_players.find(ev.peer);
			if (p->_in_game || p->_game)
			{
				g_app.not_golobby(p);
			}

			g_players.remove(ev.peer);
			for (auto p : g_players._list)
				g_app.update_players(p);
			delete ev.peer->data;
			ev.peer->data = nullptr;
			break;
		}
		}
	}
}