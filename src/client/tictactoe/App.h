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
	sf::View _view, _static_view;
};

extern App g_app;
