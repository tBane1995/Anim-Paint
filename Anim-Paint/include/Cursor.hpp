#pragma once
#include <SFML/Graphics.hpp>
#include "Window.hpp"
#include "Controls/TextInput.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"
#include <iostream>
#include "WorldToTileConverter.hpp"


class Cursor {
public:
	std::shared_ptr<sf::Cursor> _cursor;
	sf::Vector2i _position;
	std::shared_ptr<Element> _hoveredElement;
	bool _brushIsVisible;
	
	std::shared_ptr<sf::Cursor> _brushCursor;
	std::shared_ptr<sf::Cursor> _fillCursor;
	std::shared_ptr<sf::Cursor> _pickerCursor;
	std::shared_ptr<sf::Cursor> _crossCursor;
	
	Cursor();
	~Cursor();

	void update();
	void handleEvent();
	void draw();
};

extern std::shared_ptr<Cursor> cursor;