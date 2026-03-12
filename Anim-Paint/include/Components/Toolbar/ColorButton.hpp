#pragma once
#include "Controls/ButtonWithSprite.hpp"

class ColorButton : public ButtonWithSprite {
public:

	sf::Color _color;

	ColorButton(sf::Color color);
	~ColorButton();

	void draw();
};

