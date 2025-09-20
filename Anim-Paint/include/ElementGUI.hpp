#pragma once
#include "SFML/Graphics.hpp"

class ElementGUI {
public:

	ElementGUI();
	~ElementGUI();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

extern ElementGUI* ElementGUI_hovered;
extern ElementGUI* ElementGUI_pressed;