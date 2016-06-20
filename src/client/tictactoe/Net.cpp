#include "stdafx.h"
#include "Net.h"
#include "SMain.h"
#include "Player.h"

Net::Net()
{
	
}

void Net::init()
{
	_sock.create(Socket::Type::Client, _IP, _PORT, 1, 1);
}

Net::~Net()
{
	_sock.disconnect();
}

void Net::tick()
{
	ENetEvent ev;
	while (_sock.get()->get(&ev) > 0) {
		switch (ev.type) {
		case Socket::Event::Connect:
		{
			cout << "Connected\n";
			ev.peer->data = nullptr;
			connected();
			break;
		}

		case Socket::Event::Packet:
		{
			cout << "Packet\n";
			msg_process(new InPacket(ev.packet));
			break;
		}

		case Socket::Event::Disconnect:
		{
			cout << "Disconnected\n";
			delete ev.peer->data;
			ev.peer->data = nullptr;
			break;
		}
		}
	}
}

void Net::msg_process(InPacket * in)
{
	Msg msg;
	in->read(msg);
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
	}

	delete in;
}

void Net::connected()
{
	string name = s_main->_entry_name->GetText();

	OutPacket out(name.size() + 8);
	out.write(Msg::Auth, name);
	out.send(_sock.get());
}

void Net::not_auth(InPacket * in)
{
	s_main->_players.clear();
	in->read(g_player._name);

	s_main->enter_lobby();
}

void Net::not_players(InPacket * in)
{
	s_main->_players.clear();
	u16 sz;
	in->read(sz);
	for (int i = 0; i < sz; i++)
	{
		string name;
		in->read(name);

		if (name == g_player._name)
			continue;

		s_main->_players.emplace_back(name);
	}

	s_main->_scroll_box->RemoveAll();
	for (auto & name : s_main->_players)
	{
		auto btn = sfg::Button::Create(name);
		btn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&SMain::SelectPlayer, s_main, name));
		s_main->_scroll_box->Pack(btn);
	}
}

void Net::send(OutPacket * out)
{
	out->send(_sock.get());
}
