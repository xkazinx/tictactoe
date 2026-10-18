#include "stdafx.h"
#include "GUI.h"
#include "Driver.h"

void GUI::draw(sf::RenderTarget & target, sf::RenderStates states) const
{
	_ptr->Display(*g_wnd.get());
}

void GUI::init()
{
	_ptr = make_unique<sfg::SFGUI>();
	if (!_font.openFromFile("fonts/comic.ttf"))
		cout << "fonts/comic.ttf not found: run the client from deploy\\\n";
	//sfg::Context::Get().GetEngine().SetProperty<std::string>("*", "FontName", "fonts/comic.ttf");
	//_desk.SetProperty("*", "FontName", "fonts/comic.ttf");
}
