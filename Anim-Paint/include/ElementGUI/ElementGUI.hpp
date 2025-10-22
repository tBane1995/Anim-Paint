#pragma once
#include "SFML/Graphics.hpp"

class ElementGUI {
public:

	ElementGUI();
	~ElementGUI();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

extern ElementGUI* ElementGUI_hovered;
extern ElementGUI* ElementGUI_pressed;