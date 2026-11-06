#include "stdafx.h"
#include "SMain.h"
#include "SFGUI/Widgets.hpp"
#include "GUI.h"
#include "Net.h"
#include "Player.h"

#define btn_click(btn, btnid) btn->GetSignal(sfg::Widget::OnLeftClick).Connect(std::bind(&SMain::click, this, ButtonID::btnid));

string ToUtf8(const sf::String & s)
{
	return lib::utf8::FromUtf32(s.toUtf32());
}

sf::String FromUtf8(const string & s)
{
	return sf::String(lib::utf8::ToUtf32(s));
}

// A table cell (SFML 3's Rect takes a position and a size)
static sf::Rect<std::uint32_t> Cell(const std::uint32_t x, const std::uint32_t y, const std::uint32_t w, const std::uint32_t h)
{
	return { { x, y }, { w, h } };
}

SMain::SMain() : _entry_name(sfg::Entry::Create())
{
	// The background behind every window (deploy\sprites\background.jpg, from
	// art\background_1920x1080.jpg): scaled to cover the window, centred; the screen
	// stays black without it
	if (texture.loadFromFile("sprites/background.jpg"))
	{
		texture.setSmooth(true);
		_background.emplace(texture);
		const sf::Vector2f size(texture.getSize());
		const float scale = std::max(res_x / size.x, res_y / size.y);
		_background->setScale({ scale, scale });
		_background->setPosition({ std::floor((res_x - size.x * scale) / 2.f), std::floor((res_y - size.y * scale) / 2.f) });
	}
}

SMain::~SMain()
{
}

void SMain::Initialize() {
	using namespace sfg;

	// Its widgets are made once (kazilib lib::Initializes: _init is "set up ran");
	// entering it again only shows the log in
	if (!InitOnce())
	{
		for (auto & wnd : _wnds)
			g_ui._desk.Add(wnd);

		show_only(Windows::LogIn);
		return;
	}

	{
		auto & wnd = getWnd(Windows::LogIn);
		wnd = Window::Create(Window::Style::BACKGROUND | Window::Style::TITLEBAR);
		wnd->SetId("LogIn");
		wnd->SetTitle("Authentication");
		wnd->SetRequisition(sf::Vector2f(res_x / 3, res_y / 6));
		auto wrapper = Box::Create(Box::Orientation::VERTICAL, 10.f);

		auto auth_table = Table::Create();
		auth_table->SetRowSpacings(5.f);

		auto game_btn = Button::Create("Log In");
		auto label = Label::Create("Nickname:");
		_entry_name->SetId("name");
		_entry_name->SetMaximumLength((int)kNameMax);

		auth_table->Attach(label, Cell(0, 0, 1, 1));
		auth_table->Attach(_entry_name, Cell(1, 0, 3, 1), Table::FILL | Table::EXPAND);
		auth_table->Attach(game_btn, Cell(0, 1, 4, 1), Table::EXPAND | Table::FILL, Table::EXPAND);

		btn_click(game_btn, LogIn);

		wrapper->Pack(auth_table);
		wnd->Add(wrapper);
		g_ui._desk.Add(wnd);
	}

	{
		auto & wnd = getWnd(Windows::Connecting);
		wnd = Window::Create(Window::Style::BACKGROUND);
		wnd->SetId("Connecting");
		wnd->SetRequisition(sf::Vector2f(400, 100));
		g_ui._desk.Add(wnd);
		wnd->Show(false);

		auto box = Box::Create(Box::Orientation::HORIZONTAL, 10.f);
		auto sp = Spinner::Create();
		sp->SetRequisition(sf::Vector2f(30.f, 30.f));
		sp->Start();

		_connecting_label = Label::Create("Connecting to the server, please wait...");
		box->Pack(sp, false);
		box->Pack(_connecting_label);
		wnd->Add(box);
	}

	{
		auto & wnd = getWnd(Windows::Select);
		wnd = Window::Create(Window::Style::BACKGROUND | Window::Style::TITLEBAR);
		wnd->SetId("Select");
		wnd->SetTitle("Select your opponent");
		wnd->SetRequisition(sf::Vector2f(400, 200));
		g_ui._desk.Add(wnd);
		wnd->Show(false);

		_scroll_box = Box::Create(Box::Orientation::VERTICAL, 4.f);
		_scroll_box->SetRequisition(sf::Vector2f(360.f, 0.f));

		auto scrolled = ScrolledWindow::Create();
		scrolled->SetRequisition(sf::Vector2f(.0f, 160.f));
		scrolled->SetScrollbarPolicy(ScrolledWindow::HORIZONTAL_NEVER | ScrolledWindow::VERTICAL_AUTOMATIC);
		scrolled->SetPlacement(ScrolledWindow::Placement::TOP_LEFT);
		scrolled->AddWithViewport(_scroll_box);
		wnd->Add(scrolled);
	}

	{
		auto & wnd = getWnd(Windows::Msg);
		wnd = Window::Create(Window::Style::BACKGROUND);
		wnd->SetId("Msg");
		g_ui._desk.Add(wnd);
		wnd->Show(false);
		auto box = Box::Create(Box::Orientation::VERTICAL, 5.f);

		_msg_title = Frame::Create();
		_msg_title->SetAlignment(sf::Vector2f(0.5f, 0.5f));
		_msg_content = Label::Create();
		_msg_content->SetRequisition(sf::Vector2f(300.f, 0.f));
		_msg_content->SetLineWrap(true);
		_msg_title->Add(_msg_content);
		box->Pack(_msg_title);

		auto btn = Button::Create("Ok");
		btn_click(btn, Msg_Ok);
		box->Pack(btn, false);
		wnd->Add(box);
	}

	{
		auto & wnd = getWnd(Windows::Game);
		wnd = Window::Create(Window::Style::BACKGROUND);
		wnd->SetId("Game");
		wnd->SetRequisition(sf::Vector2f(420, 360));
		g_ui._desk.Add(wnd);
		wnd->Show(false);

		auto box = Box::Create(Box::Orientation::VERTICAL, 8.f);
		auto table = Table::Create();

		for (int x = 0; x < 3; x++)
		{
			for (int y = 0; y < 3; y++)
			{
				_btn[x][y] = Button::Create(" ");
				_btn[x][y]->SetId("cell" + to_string(x) + to_string(y));
				_btn[x][y]->SetRequisition(sf::Vector2f(110.f, 80.f));
				_btn[x][y]->GetSignal(Widget::OnLeftClick).Connect([this, x, y] { SelectPosition(x, y); });
				table->Attach(_btn[x][y], Cell((std::uint32_t)y, (std::uint32_t)x, 1, 1), Table::FILL | Table::EXPAND, Table::FILL | Table::EXPAND, sf::Vector2f(5.f, 5.f));
			}
		}

		_vs_label = Label::Create(" ");
		_turn_label = Label::Create(" ");
		box->Pack(_vs_label, false);
		box->Pack(table);
		box->Pack(_turn_label, false);
		wnd->Add(box);
	}

	{
		auto & wnd = getWnd(Windows::Winner);
		wnd = Window::Create(Window::Style::BACKGROUND);
		wnd->SetId("Winner");
		wnd->SetRequisition(sf::Vector2f(300, 100));
		g_ui._desk.Add(wnd);
		wnd->Show(false);

		auto box = Box::Create(Box::Orientation::VERTICAL, 5.f);
		_end_label = Label::Create(" ");
		_btn_again = Button::Create("Play Again");
		auto lobby = Button::Create("Go to Lobby");
		btn_click(_btn_again, PlayAgain);
		btn_click(lobby, Lobby);

		box->Pack(_end_label);
		box->Pack(_btn_again);
		box->Pack(lobby);
		wnd->Add(box);
	}

	show_only(Windows::LogIn);
	focus_name();
}

void SMain::Render() {
	if (_background)
		_wnd->draw(*_background);
}

void SMain::show_only(const Windows id)
{
	for (auto & wnd : _wnds)
	{
		if (wnd)
			toggle_window(wnd, false);
	}

	if (id == Windows::None)
		return;

	toggle_window(getWnd(id), true);
	center(id);
}

// Centred on the screen at its current size (the end of a game: under the board)
void SMain::center(const Windows id)
{
	auto & wnd = getWnd(id);
	const auto alloc = wnd->GetAllocation().size;
	const auto req = wnd->GetRequisition();
	const sf::Vector2f size(std::max(alloc.x, req.x), std::max(alloc.y, req.y));
	float y = (res_y - size.y) / 2.f;

	if (id == Windows::Winner)
	{
		const auto & game = getWnd(Windows::Game);
		y = std::min(game->GetAllocation().position.y + game->GetAllocation().size.y + 10.f, res_y - size.y);
	}

	wnd->SetPosition(sf::Vector2f(std::floor((res_x - size.x) / 2.f), std::floor(y)));
}

void SMain::HandleEvent(const void * const event)
{
	if (!event)
		return;
	const auto key = static_cast<const sf::Event *>(event)->getIf<sf::Event::KeyPressed>();
	if (!key)
		return;

	const bool enter = key->code == sf::Keyboard::Key::Enter;
	if (enter && is_shown(Windows::LogIn))
		click(ButtonID::LogIn);
	else if ((enter || key->code == sf::Keyboard::Key::Escape) && is_shown(Windows::Msg))
		click(ButtonID::Msg_Ok);
}

void SMain::click(const ButtonID id)
{
	switch (id)
	{
	case ButtonID::LogIn:
	{
		if (!is_shown(Windows::LogIn))
			return;

		// Spaces at either end are dropped (the server does the same)
		string name = ToUtf8(_entry_name->GetText());
		const auto b = name.find_first_not_of(' ');
		name = b == string::npos ? string() : name.substr(b, name.find_last_not_of(' ') - b + 1);
		if (name.empty())
		{
			show_msg("Log In", "Type a nickname first.", Windows::LogIn);
			return;
		}

		_entry_name->SetText(FromUtf8(name));
		_entry_name->SetCursorPosition((int)_entry_name->GetText().getSize());
		_connecting_label->SetText(FromUtf8("Connecting to " + ServerIp() + ":" + to_string(ServerPort()) + "..."));
		show_only(Windows::Connecting);
		g_net._sock->Connect();
		break;
	}

	case ButtonID::Msg_Ok:
		show_only(_msg_back);
		if (_msg_back == Windows::LogIn)
			focus_name();
		break;

	case ButtonID::PlayAgain:
		if (_asked_again)
			return;
		_asked_again = true;
		_btn_again->SetState(sfg::Widget::State::INSENSITIVE);
		_end_label->SetText(FromUtf8(ToUtf8(_end_label->GetText()) + "\nWaiting for " + g_player._to_name + "..."));
		g_net.Send(OPkt::Create(Msg::PlayAgain));
		break;

	case ButtonID::Lobby:
		g_net.Send(OPkt::Create(Msg::GoLobby));
		break;
	}
}

// The nickname box takes the typing, its cursor after the text (a name kept
// from a refused log in is edited from its end)
void SMain::focus_name()
{
	_entry_name->GrabFocus();
	_entry_name->SetCursorPosition((int)_entry_name->GetText().getSize());
}

void SMain::set_players(vector<LobbyPlayer> players)
{
	_players = std::move(players);
	_scroll_box->RemoveAll();

	if (_players.empty())
		_scroll_box->Pack(sfg::Label::Create("Nobody else is online yet: waiting for players..."));

	for (auto & p : _players)
	{
		auto btn = sfg::Button::Create(FromUtf8(p._in_game ? p._name + "  (in a game)" : p._name));
		btn->SetId("player_" + p._name);
		const string name = p._name;
		btn->GetSignal(sfg::Widget::OnLeftClick).Connect([this, name] { SelectPlayer(name); });
		_scroll_box->Pack(btn, false);
	}
}

void SMain::SelectPlayer(const string & name)
{
	if (!is_shown(Windows::Select))
		return;

	g_net.Send(OPkt::Create(Msg::SelectPlayer, name));
	_connecting_label->SetText(FromUtf8("Starting a game with " + name + "..."));
	show_only(Windows::Connecting);
}

void SMain::enter_lobby()
{
	getWnd(Windows::Select)->SetTitle(FromUtf8("Select your opponent  -  you are " + g_player._name));
	show_only(Windows::Select);
}

void SMain::show_msg(const string & title, const string & text, const Windows back)
{
	_msg_back = back;
	_msg_title->SetLabel(FromUtf8(title));
	_msg_content->SetText(FromUtf8(text));
	show_only(Windows::Msg);
}

void SMain::not_playerexists()
{
	g_net._sock->Disconnect(); // disconnect_now: no Disconnect event of its own
	show_msg("Cannot log in", "The nickname " + ToUtf8(_entry_name->GetText()) + " is already in use, try another one.", Windows::LogIn);
}

void SMain::not_disconnected()
{
	g_player._name.clear();
	g_player._to_name.clear();
	_game_finished = true;
	show_msg("Disconnected", "The connection to the server (" + ServerIp() + ":" + to_string(ServerPort()) + ") was lost or could not be made.", Windows::LogIn);
}

void SMain::not_startgame(IPkt * in)
{
	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
			_btn[x][y]->SetLabel(" ");
	}

	_game_finished = false;
	_asked_again = false;
	_btn_again->SetState(sfg::Widget::State::NORMAL);
	bool turn1 = false, turn2 = false;
	string name1, name2;
	in->Read(name1, turn1, name2, turn2);

	const bool first = name1 == g_player._name;
	g_player._to_name = first ? name2 : name1;
	_is_turn = first ? turn1 : turn2;
	// The one who starts plays X
	_identifier = _is_turn ? "X" : "O";
	const string other = _is_turn ? "O" : "X";
	_vs_label->SetText(FromUtf8("You (" + _identifier + ")  vs  " + g_player._to_name + " (" + other + ")"));

	update_turn_label();
	show_only(Windows::Game);
}

void SMain::SelectPosition(const int x, const int y)
{
	if (_game_finished || !_is_turn)
		return;

	const string s = ToUtf8(_btn[x][y]->GetLabel());
	if (s == "X" || s == "O")
		return;

	g_net.Send(OPkt::Create(Msg::Position, x, y));
}

void SMain::update_turn_label()
{
	if (_game_finished)
		return;

	_turn_label->SetText(FromUtf8(_is_turn ? "It's your turn (" + _identifier + ")" : "It's " + g_player._to_name + "'s turn"));
}

void SMain::not_position(IPkt * in)
{
	string name1, name2;
	bool turn1 = false, turn2 = false;
	in->Read(name1, turn1, name2, turn2);
	_is_turn = name1 == g_player._name ? turn1 : turn2;

	for (int x = 0; x < 3; x++)
	{
		for (int y = 0; y < 3; y++)
		{
			string identifier;
			in->Read(identifier);
			_btn[x][y]->SetLabel(FromUtf8(identifier));
		}
	}

	update_turn_label();
}

void SMain::not_playeringame()
{
	show_msg("Cannot start the game", "That player is already in a game. Wait for it to end or pick another one.", Windows::Select);
}

void SMain::not_playergone()
{
	show_msg("Cannot start the game", "That player is no longer online.", Windows::Select);
}

void SMain::show_end(const string & text)
{
	_game_finished = true;
	_is_turn = false;
	_turn_label->SetText(FromUtf8(text));
	_end_label->SetText(FromUtf8(text));
	toggle_window(getWnd(Windows::Winner), true);
	center(Windows::Winner);
}

void SMain::not_winner(IPkt * in)
{
	in->Read(_winner_name);
	show_end(_winner_name == g_player._name ? "You won!" : _winner_name + " won the game!");
}

void SMain::not_gameended()
{
	_winner_name.clear();
	show_end("It's a draw!");
}

void SMain::not_playagain(IPkt * in)
{
	string name;
	in->Read(name);
	if (!_asked_again)
		_end_label->SetText(FromUtf8(ToUtf8(_end_label->GetText()) + "\n" + name + " wants to play again!"));
}

void SMain::not_golobby()
{
	_game_finished = false;
	_winner_name.clear();
	_is_turn = false;
	g_player._to_name.clear();
	enter_lobby();
}

void SMain::not_opponentleft(IPkt * in)
{
	string name;
	in->Read(name);
	not_golobby();
	show_msg("Game over", name + " left the game.", Windows::Select);
}
