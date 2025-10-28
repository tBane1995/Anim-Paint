#include "Mouse.hpp"
#include "SFML/Graphics.hpp"

Cursor::Cursor() {

	_cursor = new sf::Cursor(sf::Cursor::Type::Arrow);
	_mousePosition = sf::Mouse::getPosition(*window);
	_worldMousePosition = window->mapPixelToCoords(_mousePosition);
	window->setMouseCursor(*_cursor);
	_hoveredElementGUI = nullptr;
	_ico = nullptr;
}

Cursor::~Cursor() {
	delete _cursor;

	if (_ico != nullptr)
		delete _ico;
}

void Cursor::update() {
	_mousePosition = sf::Mouse::getPosition(*window);
	_worldMousePosition = window->mapPixelToCoords(_mousePosition);
}

void Cursor::handleEvent(const sf::Event& event) {

	if (_hoveredElementGUI!= canvas && _hoveredElementGUI == ElementGUI_hovered)
		return;

	_hoveredElementGUI = ElementGUI_hovered;

	if (_cursor != nullptr) {
		delete _cursor;
		_cursor = nullptr;
	}

	if (dynamic_cast<TextInput*>(_hoveredElementGUI) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = new sf::Cursor(sf::Cursor::Type::Text);
		window->setMouseCursor(*_cursor);
		return;
	}

	sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
	if (lasso->_state == LassoState::Selected || lasso->_state == LassoState::Moving) {
		std::wcout << "lasso selected or moved\n";
		if (lasso->clickOnSelection(tile)) {
			std::wcout << "on selection\n";
			window->setMouseCursorVisible(true);
			_cursor = new sf::Cursor(sf::Cursor::Type::SizeAll);
			window->setMouseCursor(*_cursor);
			return;
		}
	}

	if (_hoveredElementGUI == canvas) {
		if (toolbar->_toolType == ToolType::Brush) {
			window->setMouseCursorVisible(false);
		}
		else if (toolbar->_toolType == ToolType::Eraser) {
			window->setMouseCursorVisible(false);
		}
		else {
			window->setMouseCursorVisible(true);
			_cursor = new sf::Cursor(sf::Cursor::Type::Cross);
			window->setMouseCursor(*_cursor);
		}

		return;
	}

	window->setMouseCursorVisible(true);
	_cursor = new sf::Cursor(sf::Cursor::Type::Arrow);
	window->setMouseCursor(*_cursor);
}

void Cursor::draw() {
	if (_ico != nullptr) {
		window->draw(*_ico);
	}
}

Cursor* cursor = nullptr;