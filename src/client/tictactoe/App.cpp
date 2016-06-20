#include "stdafx.h"
#include "App.h"
#include "Driver.h"
#include <thread>
#include <chrono>
#include "GUI.h"
#include "Scene.h"
#include "Net.h"

void App::init()
{
	g_ui.init();
	g_wnd.init();
	g_scene.init();

	_dummy_text = sf::Text("aeou", g_ui._font);
	_dummy_text.setPosition(sf::Vector2f(-100.f, -100.f));
	_view = g_wnd.get()->getDefaultView();
	_static_view = g_wnd.get()->getDefaultView();

	loop();
}

void App::loop()
{
	auto wnd = g_wnd.get();

	while (wnd->isOpen())
	{
		Clock.seed();
		g_net.tick();
		tick();
		handle_events();
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void App::tick()
{
	auto wnd = g_wnd.get();

	wnd->clear(sf::Color::Black);
	draw();
	wnd->display();
}

void App::handle_events()
{
	auto wnd = g_wnd.get();

	sf::Event event;
	while (wnd->pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
			wnd->close();

		g_scene.handle_event(event);
		g_ui._desk.HandleEvent(event);
	}
}

void App::draw()
{
	static Time::Val frametime = Clock.now() - 1000 / 60;
	if (!Clock.elapsed(frametime, 1000 / 60)) {
		return;
	}

	auto wnd = g_wnd.get();
	wnd->draw(g_scene);
	wnd->draw(_dummy_text);
	wnd->draw(g_ui);
	g_ui._desk.Update(Clock.deltaf());
}
