#pragma once
#include "Controls/ButtonWithSprite.hpp"

class LargeColorButton : public ButtonWithSprite {
public:

	sf::Color _color;
	sf::Shader _shader;
	LargeColorButton(sf::Color color);
	~LargeColorButton();
	
	void setColor(sf::Color color);

	void draw();
};

