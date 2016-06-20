#pragma once
#include "SFGUI/Desktop.hpp"

class GUI : public sf::Drawable
{
public:
	virtual void draw(sf::RenderTarget & target, sf::RenderStates states) const override;

	void init();

	uPtr<sfg::SFGUI> _ptr;
	sf::Font _font;
	sfg::Desktop _desk;
};

extern GUI g_ui;