#pragma once
#include <SFML/Graphics.hpp>
#include "Window.hpp"
#include "ElementGUI/TextInput.hpp"
#include "Components/Toolbar.hpp"
#include "Tools/Lasso.hpp"
#include "Components/Canvas.hpp"
#include <iostream>
#include "WorldToTileConverter.hpp"


class Cursor {
public:
	std::shared_ptr<sf::Cursor> _cursor;
	sf::Vector2i _mousePosition;
	sf::Vector2i _worldMousePosition;
	std::shared_ptr<ElementGUI> _hoveredElementGUI;
	sf::Vector2i _offset;
	std::shared_ptr<Texture> _ico;
	bool _brushIsVisible;
	
	std::shared_ptr<sf::Cursor> _fillCursor;
	std::shared_ptr<sf::Cursor> _pickerCursor;
	
	Cursor();
	~Cursor();

	void update();
	void handleEvent(const sf::Event& event);
	void draw();
};

extern std::shared_ptr<Cursor> cursor;