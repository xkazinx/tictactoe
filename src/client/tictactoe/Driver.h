#pragma once


//Container for SFML-Window
struct Driver
{
	unique_ptr<sf::RenderWindow> _wnd;
	sf::RenderWindow * get(){ return _wnd.get(); }

	void init();
	void close();
};

extern Driver g_wnd;
