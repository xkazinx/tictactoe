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
		Max
	};
	sfg::Window::Ptr _wnds[(int)Windows::Max];
	sfg::Window::Ptr & getWnd(Windows id){ return _wnds[(int)id]; }

	enum class ButtonID
	{
		LogIn,

	};
	void click(ButtonID id);

};

