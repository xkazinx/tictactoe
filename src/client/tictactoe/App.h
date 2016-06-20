#pragma once

//Game handler
class App
{
public:
	void init();
	void loop();
	void tick();
	sf::View _view, _static_view;
};

