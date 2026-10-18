#pragma once

//Game handler
class App
{
public:
	static constexpr Time::Val kFrameMs = 1000 / 60;

	void init();
	void loop();
	void frame(const float seconds);
	void handle_events();
	// One event, real or Testerino's, through the same path
	void process_event(const sf::Event & event);

	optional<sf::Text> _dummy_text; // SFML 3: a text needs its font to be made
};

extern App g_app;
