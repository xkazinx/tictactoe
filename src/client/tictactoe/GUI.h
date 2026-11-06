#pragma once
#include "SFGUI/Desktop.hpp"

struct GUI : sf::Drawable
{
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

	void init();

	unique_ptr<sfg::SFGUI> _ptr;
	sf::Font _font;
	sfg::Desktop _desk;
};

extern GUI g_ui;