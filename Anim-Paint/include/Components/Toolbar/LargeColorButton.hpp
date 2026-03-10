#pragma once
#include "Controls/ButtonWithSprite.hpp"

class LargeColorButton : public ButtonWithSprite {
public:

	sf::Color _color;

	LargeColorButton(sf::Color color);
	~LargeColorButton();
	
	void setPosition(sf::Vector2i position);
	void setColor(sf::Color color);
	void draw();
};

