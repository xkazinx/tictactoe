#include "stdafx.h"
#include "GUI.h"
#include "Driver.h"

void GUI::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	_ptr->Display(*g_wnd.get());
}

void GUI::init()
{
	_ptr = uMake<sfg::SFGUI>();
	_font.loadFromFile("fonts/comic.ttf");
	//sfg::Context::Get().GetEngine().SetProperty<std::string>("*", "FontName", "fonts/comic.ttf");
	//_desk.SetProperty("*", "FontName", "fonts/comic.ttf");
}
