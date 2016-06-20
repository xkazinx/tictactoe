#include "stdafx.h"
#include "Driver.h"

void Driver::init()
{
	_wnd = uMake<sf::RenderWindow>(sf::VideoMode(res_x, res_y), "Tic Tac Toe - Online");
}

void Driver::close()
{
	_wnd->close();
}
