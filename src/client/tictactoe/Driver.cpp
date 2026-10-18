#include "stdafx.h"
#include "Driver.h"

void Driver::init()
{
	// title=<text>: told apart from other clients; background: opens behind the
	// window that has focus and leaves it the focus (tests running while the PC is used)
	const string extra = lib::init::Value("title");
	const string title = "Tic Tac Toe - Online" + (extra.empty() ? string() : " - " + extra);
	const bool background = lib::init::Flag("background", false);
	const HWND before = background ? GetForegroundWindow() : nullptr;

	_wnd = make_unique<sf::RenderWindow>(sf::VideoMode({ res_x, res_y }), title, sf::Style::Titlebar | sf::Style::Close);

	if (background)
	{
		SetWindowPos((HWND)_wnd->getNativeHandle(), HWND_BOTTOM, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_NOACTIVATE);
		if (before)
			SetForegroundWindow(before);
	}
}

void Driver::close()
{
	_wnd->close();
}
