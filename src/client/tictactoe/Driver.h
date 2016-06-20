#pragma once


//Container for SFML-Window
class Driver
{
public:
	uPtr<sf::RenderWindow> _wnd;
	sf::RenderWindow * get(){ return _wnd.get(); }

	void init();
	void close();
};

extern Driver g_wnd;
