#pragma once
#include "SFML/Graphics.hpp"

class ElementGUI {
public:

	ElementGUI();
	~ElementGUI();

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};

extern ElementGUI* ElementGUI_hovered;
extern ElementGUI* ElementGUI_pressed;