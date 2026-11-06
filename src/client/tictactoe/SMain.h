#pragma once
#include "Scene.h"
#include "SFGUI/Widgets.hpp"

struct SMain : Scene
{
	SMain();
	~SMain();
	void Initialize() override;
	void Render() override;
	void HandleEvent(const void * event) override; // an sf::Event

	enum class Windows
	{
		None = -1,
		LogIn,
		Connecting,
		Select,
		Msg,
		Game,
		Winner,
		Max
	};
	sfg::Window::Ptr _wnds[Under(Windows::Max)];
	sfg::Window::Ptr & getWnd(const Windows id) { return _wnds[Under(id)]; }
	bool is_shown(const Windows id) const { return _wnds[Under(id)] && _wnds[Under(id)]->IsLocallyVisible(); }
	// Hides every window but `id` (None: all of them)
	void show_only(const Windows id);
	void center(const Windows id);

	enum class ButtonID
	{
		LogIn,
		Msg_Ok,
		PlayAgain,
		Lobby,
	};

	void click(const ButtonID id);
	void enter_lobby();

	// Log in
	sfg::Entry::Ptr _entry_name;
	sfg::Label::Ptr _connecting_label;
	void focus_name();

	// Lobby
	struct LobbyPlayer
	{
		string _name;
		bool _in_game = false;
	};
	vector<LobbyPlayer> _players;
	sfg::Box::Ptr _scroll_box;
	void set_players(vector<LobbyPlayer> players);
	void SelectPlayer(const string & name);

	// Messages: Ok goes back to _msg_back
	sfg::Label::Ptr _msg_content;
	sfg::Frame::Ptr _msg_title;
	Windows _msg_back = Windows::LogIn;
	void show_msg(const string & title, const string & text, const Windows back);

	// Game
	sfg::Button::Ptr _btn[3][3]; // [row][column]
	sfg::Label::Ptr _vs_label;
	sfg::Label::Ptr _turn_label;
	bool _is_turn = false;
	bool _game_finished = false;
	string _identifier;
	void SelectPosition(const int x, const int y);
	void update_turn_label();

	// End of a game
	sfg::Label::Ptr _end_label;
	sfg::Button::Ptr _btn_again;
	string _winner_name;
	bool _asked_again = false;
	void show_end(const string & text);

	// Packets
	Msg _last_msg = Msg::Auth;
	void not_playerexists();
	void not_startgame(IPkt * in);
	void not_position(IPkt * in);
	void not_playeringame();
	void not_playergone();
	void not_winner(IPkt * in);
	void not_gameended();
	void not_playagain(IPkt * in);
	void not_golobby();
	void not_opponentleft(IPkt * in);
	void not_disconnected();

	sf::Texture texture;
	optional<sf::Sprite> _background; // SFML 3: a sprite needs its texture to be made
};

extern SMain * s_main;

// UTF-8 both ways between the wire / std::string and SFGUI's sf::String
string ToUtf8(const sf::String & s);
sf::String FromUtf8(const string & s);
