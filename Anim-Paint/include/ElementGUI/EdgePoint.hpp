#pragma once
#include "ElementGUI/ElementGUI.hpp"


class EdgePoint : public ElementGUI {
public:
	sf::IntRect _rect;

	EdgePoint(sf::Vector2i position);
	~EdgePoint();

	void setPosition(sf::Vector2i position);
	sf::Vector2i getPosition();
	sf::Vector2i getSize();
	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};