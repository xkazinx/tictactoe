#include "stdafx.h"
#include "Net.h"
#include "SMain.h"
#include "Player.h"

Net::Net()
{
}

void Net::init()
{
	_sock = make_unique<SocketHandler>(Socket::Type::Client, ServerIp(), ServerPort(), 1, 1);
	cout << "Server: " << ServerIp() << ":" << ServerPort() << "\n";
}

Net::~Net()
{
	if (_sock)
		_sock->Disconnect();
}

void Net::tick()
{
	ENetEvent ev;
	for (auto e = _sock->Service(&ev); e != Socket::Event::None; e = _sock->Service(&ev))
	{
		switch (e)
		{
		case Socket::Event::Connect:
			cout << "Connected to server\n";
			connected();
			break;

		case Socket::Event::Packet:
		{
			IPkt in(ev.packet); // destroys the enet packet when read
			msg_process(&in);
			break;
		}

		case Socket::Event::Disconnect:
			cout << "Disconnected from server\n";
			if (s_main)
				s_main->not_disconnected();
			break;

		default:
			break;
		}
	}
}

void Net::msg_process(IPkt * in)
{
	Msg msg;
	in->Read(msg);
	s_main->_last_msg = msg;
	switch (msg)
	{
	case Msg::Auth:
		not_auth(in);
		break;

	case Msg::Players:
		not_players(in);
		break;

	case Msg::PlayerExists:
		s_main->not_playerexists();
		break;

	case Msg::StartGame:
		s_main->not_startgame(in);
		break;

	case Msg::Position:
		s_main->not_position(in);
		break;

	case Msg::PlayerInGame:
		s_main->not_playeringame();
		break;

	case Msg::Winner:
		s_main->not_winner(in);
		break;

	case Msg::GoLobby:
		s_main->not_golobby();
		break;

	case Msg::GameEnded:
		s_main->not_gameended();
		break;

	case Msg::PlayAgain:
		s_main->not_playagain(in);
		break;

	case Msg::PlayerGone:
		s_main->not_playergone();
		break;

	case Msg::OpponentLeft:
		s_main->not_opponentleft(in);
		break;

	default:
		break;
	}
}

void Net::connected()
{
	Send(OPkt::Create(Msg::Auth, ToUtf8(s_main->_entry_name->GetText())));
}

void Net::not_auth(IPkt * in)
{
	in->Read(g_player._name);
	s_main->enter_lobby();
}

void Net::not_players(IPkt * in)
{
	u16 sz = 0;
	in->Read(sz);

	vector<SMain::LobbyPlayer> players;

	for (int i = 0; i < sz; i++)
	{
		SMain::LobbyPlayer p;
		in->Read(p._name, p._in_game);

		if (p._name != g_player._name)
			players += p;
	}

	s_main->set_players(std::move(players));
}

void Net::Send(const OPktPtr & out)
{
	out->Send(_sock->Get());
}
