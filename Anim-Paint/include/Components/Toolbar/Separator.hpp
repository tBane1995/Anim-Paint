#pragma once
#include "SFML/Graphics.hpp"

class Separator {
public:
	sf::IntRect _rect;
	Separator();
	~Separator();

	void setPosition(sf::Vector2i position);
	sf::Vector2i getSize();
	void draw();
};
