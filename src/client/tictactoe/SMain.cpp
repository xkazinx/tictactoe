#include "stdafx.h"
#include "SMain.h"
#include "SFGUI/Widgets.hpp"
#include "GUI.h"
#include "Net.h"
#include "Player.h"

#define btn_click(btn, btnid) btn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&SMain::click, this, ButtonID::btnid));

SMain::SMain() : _entry_name(sfg::Entry::Create())
{

}


SMain::~SMain()
{
}

void SMain::initialize() {
	using namespace sfg;

	{
		auto & wnd = getWnd(Windows::LogIn);
		wnd = sfg::Window::Create(sfg::Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR);
		wnd->SetTitle("Authentication");
		wnd->SetRequisition(sf::Vector2f(res_x / 3, res_y / 6));
		wnd->SetPosition(sf::Vector2f((res_x / 2) - ((res_x / 2) / 3), (res_y / 2) - ((res_y / 2) / 6)));
		auto wrapper = sfg::Box::Create(sfg::Box::Orientation::VERTICAL, 10.f);

		auto auth_table = Table::Create();
		auth_table->SetRowSpacings(5.f);

		auto game_btn = Button::Create(L"Log In");
		auto label = Label::Create(L"Nickname:");

		auth_table->Attach(label, sf::Rect<sf::Uint32>(0, 0, 1, 1));
		auth_table->Attach(_entry_name, sf::Rect<sf::Uint32>(1, 0, 3, 1), sfg::Table::FILL);
		auth_table->Attach(game_btn, sf::Rect<sf::Uint32>(0, 1, 3, 1), 3, sfg::Table::EXPAND | sfg::Table::EXPAND);

		btn_click(game_btn, LogIn);

		wrapper->Pack(auth_table);
		wnd->Add(wrapper);
		g_ui._desk.Add(wnd);
	}

	{
		auto & wnd = getWnd(Windows::Connecting);
		wnd = sfg::Window::Create(Window::Style::BACKGROUND);
		wnd->SetTitle("Connecting to Server");
		wnd->SetRequisition(sf::Vector2f(400, 150));
		wnd->SetPosition(sf::Vector2f((res_x / 2) - (400 / 2), (res_y / 2) - 150 / 2));
		g_ui._desk.Add(wnd);
		wnd->Show(false);

		auto box = Box::Create(Box::Orientation::HORIZONTAL);
		auto sp = Spinner::Create();
		sp->Start();
		
		auto align = Alignment::Create();
		align->SetScale(sf::Vector2f(0.5f, 0.5f));
		align->SetAlignment(sf::Vector2f(5.f, .5f));
		align->Add(sp);

		box->Pack(align);
		box->Pack(Label::Create(L"Connecting to Server, please wait..."));
		wnd->Add(box);
	}

	{
		auto & wnd = getWnd(Windows::Select);
		wnd = sfg::Window::Create(Window::Style::BACKGROUND | sfg::Window::Style::TITLEBAR);
		wnd->SetTitle("Select your opponent");
		wnd->SetRequisition(sf::Vector2f(400, 150));
		wnd->SetPosition(sf::Vector2f((res_x / 2) - (400 / 2), (res_y / 2) - 150 / 2));
		g_ui._desk.Add(wnd);
		wnd->Show(false);

		_scroll_box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
		
		/*for (int j = 0; j < 20; j++) {
			m_scrolled_window_box->Pack(sfg::Button::Create(L"Test"));
		}*/

		auto m_scrolled_window = sfg::ScrolledWindow::Create();
		m_scrolled_window->SetRequisition(sf::Vector2f(.0f, 160.f));
		m_scrolled_window->SetScrollbarPolicy(sfg::ScrolledWindow::HORIZONTAL_AUTOMATIC | sfg::ScrolledWindow::VERTICAL_AUTOMATIC);
		m_scrolled_window->SetPlacement(sfg::ScrolledWindow::Placement::TOP_LEFT);
		m_scrolled_window->AddWithViewport(_scroll_box);

		auto scrollbar = sfg::Scrollbar::Create();
		scrollbar->SetRange(.0f, 100.f);

		auto m_scale = sfg::Scale::Create();
		m_scale->SetAdjustment(scrollbar->GetAdjustment());
		m_scale->SetRequisition(sf::Vector2f(100.f, .0f));
		wnd->Add(m_scrolled_window);
	}

	{
		auto & wnd = getWnd(Windows::Msg);
		wnd = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
		wnd->SetRequisition(sf::Vector2f(0, 150));
		g_ui._desk.Add(wnd);
		wnd->Show(false);
		auto box = Box::Create(Box::Orientation::VERTICAL, 5.f);
		{
			auto & frame = _msg_title;
			frame = sfg::Frame::Create();
			frame->SetAlignment(sf::Vector2f(0.5f, 0.5f));
			auto & label = _msg_content;
			label = sfg::Label::Create();
			label->SetRequisition(sf::Vector2f(300.f, 0.f));
			label->SetLineWrap(true);

			frame->Add(label);
			box->Pack(frame);
		}

		{
			auto btn = Button::Create(L"Ok");
			btn_click(btn, Msg_Ok);
			//align->Add(btn);
			box->Pack(btn, false);
		}
		wnd->Add(box);
	}

	{
		auto & wnd = getWnd(Windows::Game);
		wnd = sfg::Window::Create(sfg::Window::Style::BACKGROUND);
		wnd->SetRequisition(sf::Vector2f(500, 500));
		wnd->SetPosition(sf::Vector2f(res_x / 2 - 500/2, res_y / 2 - 500/2));
		g_ui._desk.Add(wnd);
		wnd->Show(false);

		auto box = sfg::Box::Create(sfg::Box::Orientation::VERTICAL);
		auto table = sfg::Table::Create();
		
		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				_btn[x][y] = sfg::Button::Create("");	
				_btn[x][y]->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&SMain::SelectPosition, this, x, y));
			}
		}

		table->Attach(_btn[0][0], sf::Rect<sf::Uint32>(0, 0, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));
		table->Attach(_btn[1][0], sf::Rect<sf::Uint32>(0, 3, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));
		table->Attach(_btn[2][0], sf::Rect<sf::Uint32>(0, 6, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));

		table->Attach(_btn[0][1], sf::Rect<sf::Uint32>(1, 0, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));
		table->Attach(_btn[1][1], sf::Rect<sf::Uint32>(1, 3, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));
		table->Attach(_btn[2][1], sf::Rect<sf::Uint32>(1, 6, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));

		table->Attach(_btn[0][2], sf::Rect<sf::Uint32>(2, 0, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));
		table->Attach(_btn[1][2], sf::Rect<sf::Uint32>(2, 3, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));
		table->Attach(_btn[2][2], sf::Rect<sf::Uint32>(2, 6, 1, 3), sfg::Table::FILL | sfg::Table::EXPAND, sfg::Table::FILL, sf::Vector2f(10.f, 10.f));

		_turn_label = sfg::Label::Create("");
		box->Pack(table);
		box->Pack(_turn_label);
		wnd->Add(box);
	}
}

void SMain::display() const {

}

void SMain::click(ButtonID id)
{
	switch (id)
	{
	case ButtonID::LogIn:
	{
		string name = _entry_name->GetText();
		if (name.size() == 0)
			return;

		toggle_window(getWnd(Windows::LogIn), false);
		toggle_window(getWnd(Windows::Connecting), true);
		g_net._sock.connect();
		break;
	}

	case ButtonID::Msg_Ok:
		if (_last_msg == Msg::PlayerExists)
		{
			toggle_window(getWnd(Windows::Msg), false);
			toggle_window(getWnd(Windows::LogIn), true);
		}
		else if (_last_msg == Msg::PlayerInGame)
		{
			toggle_window(getWnd(Windows::Msg), false);
			toggle_window(getWnd(Windows::Select), true);
		}
		break;
	}
}

void SMain::SelectPlayer(string name)
{
	OutPacket out(name.size() + 2 + 8);
	out.write(Msg::SelectPlayer);
	out.write(name);
	g_net.send(&out);

	toggle_window(getWnd(Windows::Connecting), true);
	toggle_window(getWnd(Windows::Select), false);
}

void SMain::enter_lobby()
{
	toggle_window(getWnd(Windows::Connecting), false);
	toggle_window(getWnd(Windows::Select), true);
}

void SMain::set_notify(string title, string txt)
{
	_msg_title->SetLabel(title);
	_msg_content->SetText(txt);
}

void SMain::not_playerexists()
{
	set_notify("Cannot connect to server", "The name you provided to LogIn is already in use, try another one");
	toggle_window(getWnd(SMain::Windows::Connecting), false);
	toggle_window(getWnd(SMain::Windows::Msg), true);
	g_net._sock.disconnect();
}

void SMain::not_startgame(InPacket * in)
{
	bool turn1, turn2;
	string name1, name2;
	in->read(name1, turn1, name2, turn2);

	string name;
	if (name1 == g_player._name)
		name = name2;
	else name = name1;
	g_player._to_name = name;

	if (name1 == g_player._name)
	{
		_is_turn = turn1;
	}
	else
	{
		_is_turn = turn2;
	}

	if (_is_turn)
	{
		_identifier = "X";
	}
	else
	{
		_identifier = "O";
	}

	update_turn_label();

	toggle_window(getWnd(SMain::Windows::Connecting), false);
	toggle_window(getWnd(SMain::Windows::Game), true);
}

void SMain::SelectPosition(int x, int y)
{
	if (!_is_turn)
		return;

	string s = _btn[x][y]->GetLabel();
	if (s == "X" || s == "O")
		return;

	cout << "Selecciono posicion " << x << y << std::endl;
	OutPacket out(16);
	out.write(Msg::Position, x, y);
	g_net.send(&out);
	//_btn[x][y]->SetLabel("X");
}

void SMain::update_turn_label()
{
	string s;
	if (_is_turn)
		s = "It's your turn";
	else
		s = "It's your opponent's turn";

	_turn_label->SetText(s);
}

void SMain::not_position(InPacket * in)
{
	string name1, name2;
	bool turn1, turn2;
	in->read(name1, turn1, name2, turn2);

	if (name1 == g_player._name)
	{
		_is_turn = turn1;
	}
	else
	{
		_is_turn = turn2;
	}

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			string identifier;
			in->read(identifier);
			_btn[x][y]->SetLabel(identifier);
		}
	}

	update_turn_label();
}

void SMain::not_playeringame()
{
	set_notify("Cannot start game", "The player you selected is already on a game. Please wait or select another.");
	toggle_window(getWnd(SMain::Windows::Connecting), false);
	toggle_window(getWnd(SMain::Windows::Msg), true);
}