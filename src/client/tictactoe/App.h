#pragma once

//Game handler
class App
{
public:
	void init();
	void loop();
	void tick();
	void handle_events();
	void draw();

	sf::Text _dummy_text;
};

extern App g_app;
