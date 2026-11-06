#include "stdafx.h"
#include "App.h"
#include "Driver.h"
#include <thread>
#include <chrono>
#include "GUI.h"
#include "Scene.h"
#include "Net.h"
#include "Testerino.h"

void App::init()
{
	g_net.init();
	g_ui.init();
	g_wnd.init();
	g_scene.Init();

	//A text must be drawn somewhere on the screen for sfgui fonts to work
	_dummy_text.emplace(g_ui._font, "aeou");
	_dummy_text->setPosition(sf::Vector2f(-100.f, -100.f));

	testerino::Start();
	loop();
	testerino::Stop();
	g_scene.Clear();
}

void App::loop()
{
	auto wnd = g_wnd.get();
	Time::Val last_frame = Clock.now() - kFrameMs;

	while (wnd->isOpen())
	{
		Clock.seed();
		g_net.tick();
		handle_events();

#ifdef __EMSCRIPTEN__
		// A browser: one frame per pass; display() waits for the browser's next
		// animation frame (the page's refresh), so nothing to wait for here
		frame(Clock.elapsed(last_frame) / 1000.f);
		last_frame = Clock.now();
		continue;
#endif

		// One whole frame (clear, draw, present) every kFrameMs: presenting a cleared
		// frame between them made the window flicker black
		const Time::Val since = Clock.elapsed(last_frame);
		if (since < kFrameMs)
		{
			std::this_thread::sleep_for(std::chrono::milliseconds(1));
			continue;
		}

		last_frame = Clock.now();
		frame(since / 1000.f);
	}
}

void App::frame(const float seconds)
{
	auto wnd = g_wnd.get();
	testerino::Pump();
	g_ui._desk.Update(seconds);

	wnd->clear(sf::Color::Black);
	g_scene.Render();

	//A text must be drawn somewhere on the screen for sfgui fonts to work
	if (_dummy_text)
		wnd->draw(*_dummy_text);

	wnd->draw(g_ui);
	testerino::AfterDraw(*wnd);
	wnd->display();
}

void App::handle_events()
{
	auto wnd = g_wnd.get();

	while (const optional<sf::Event> event = wnd->pollEvent())
	{
		process_event(*event);
		if (!wnd->isOpen())
			return;
	}
}

void App::process_event(const sf::Event & event)
{
	if (event.is<sf::Event::Closed>())
	{
		g_wnd.get()->close();
		return;
	}

	g_scene.HandleEvent(event);
	g_ui._desk.HandleEvent(event);
}
