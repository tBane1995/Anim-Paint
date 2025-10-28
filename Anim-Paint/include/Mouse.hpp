#pragma once
#include <SFML/Graphics.hpp>
#include "Window.hpp"
#include "ElementGUI/TextInput.hpp"
#include "Tools/Toolbar.hpp"
#include "Tools/Lasso.hpp"
#include "Canvas.hpp"
#include <iostream>
#include "WorldToTileConverter.hpp"


class Cursor {
public:
	sf::Cursor* _cursor;
	sf::Vector2i _mousePosition;
	sf::Vector2f _worldMousePosition;
	ElementGUI* _hoveredElementGUI;
	sf::Sprite* _ico;
	
	
	Cursor();
	~Cursor();

	void update();
	void handleEvent(const sf::Event& event);
	void draw();
};

extern Cursor* cursor;