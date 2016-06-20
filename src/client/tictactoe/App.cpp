#include "stdafx.h"
#include "App.h"
#include "Driver.h"
#include <thread>
#include <chrono>

void App::init()
{
	g_wnd.init();

	_view = g_wnd.get()->getDefaultView();
	_static_view = g_wnd.get()->getDefaultView();

	loop();
}

void App::loop()
{
	auto wnd = g_wnd.get();

	while (wnd->isOpen())
	{
		tick();


		sf::Event event;
		while (wnd->pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
				wnd->close();
		}
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void App::tick()
{
	auto wnd = g_wnd.get();

	wnd->clear(sf::Color::Black);
	sf::CircleShape asd(30.f);
	asd.setFillColor(sf::Color::Red);
	wnd->draw(asd);
	wnd->display();
}
