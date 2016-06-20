#pragma once
#include "Scene.h"
#include "SFGUI/Widgets.hpp"

class SMain : public Scene
{
public:
	SMain();
	~SMain();
	void initialize();
	void display() const;

	sfg::Entry::Ptr _entry_name;

	enum class Windows
	{
		None = -1,
		LogIn,
		Connecting,
		Select,
		Msg,
		Game,
		Max
	};
	sfg::Window::Ptr _wnds[(int)Windows::Max];
	sfg::Window::Ptr & getWnd(Windows id){ return _wnds[(int)id]; }

	enum class ButtonID
	{
		LogIn,
		Msg_Ok,

	};
	void click(ButtonID id);
	void enter_lobby();

	Vector<string> _players;

	sfg::Box::Ptr _scroll_box;
	sfg::Label::Ptr _msg_content;
	sfg::Frame::Ptr _msg_title;
	void set_notify(string title, string txt);
	void not_playerexists();
	void SelectPlayer(string name);
	void not_startgame(InPacket * in);

	sfg::Button::Ptr _btn[3][3];
	void SelectPosition(int x, int y);

	sfg::Label::Ptr _turn_label;
	bool _is_turn;
	void update_turn_label();
	string _identifier;
	void not_position(InPacket * in);
	void not_playeringame();
	Msg _last_msg;
};

extern SMain * s_main;