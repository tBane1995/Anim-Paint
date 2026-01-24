 #include "Cursor.hpp"
#include "SFML/Graphics.hpp"
#include "Dialogs/FileDialog.hpp"
#include "Canvas.hpp"
#include "Tools/Brush.hpp"
#include "Dialogs/Palette.hpp"

Cursor::Cursor() {

	_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Arrow);
	_mousePosition = sf::Mouse::getPosition(*window);
	_worldMousePosition = sf::Vector2i(window->mapPixelToCoords(_mousePosition));
	window->setMouseCursor(*_cursor);
	_hoveredElementGUI = nullptr;
	_brushIsVisible = false;

	// cursors
	{
		sf::Image img = getTexture(L"tex\\cursor\\fill.png")->_texture->copyToImage();
		_fillCursor = std::make_shared<sf::Cursor>(img.getPixelsPtr(), img.getSize(), sf::Vector2u(6, 26));
	}

	{
		sf::Image img = getTexture(L"tex\\cursor\\picker.png")->_texture->copyToImage();
		_pickerCursor = std::make_shared<sf::Cursor>(img.getPixelsPtr(), img.getSize(), sf::Vector2u(5, 26));
	}
}

Cursor::~Cursor() {

}

void Cursor::update() {
	_mousePosition = sf::Mouse::getPosition(*window);
	_worldMousePosition = sf::Vector2i(window->mapPixelToCoords(_mousePosition));


}

void Cursor::handleEvent(const sf::Event& event) {

	brush->setPosition(worldToTile(_worldMousePosition, canvas->_position, canvas->_zoom, canvas->_zoom_delta));


	if (_hoveredElementGUI != canvas && _hoveredElementGUI == ElementGUI_hovered)
		return;

	_hoveredElementGUI = ElementGUI_hovered;


	if (dynamic_cast<TextInput*>(_hoveredElementGUI.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Text);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (dynamic_cast<Button*>(_hoveredElementGUI.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Arrow);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}
	
	if (_hoveredElementGUI == canvas->_point_left_top || ElementGUI_pressed == canvas->_point_left_top) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTopLeft);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (_hoveredElementGUI == canvas->_point_top || ElementGUI_pressed == canvas->_point_top) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTop);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (_hoveredElementGUI == canvas->_point_right_top || ElementGUI_pressed == canvas->_point_right_top) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTopRight);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (_hoveredElementGUI == canvas->_point_left || ElementGUI_pressed == canvas->_point_left) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeLeft);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (_hoveredElementGUI == canvas->_point_right || ElementGUI_pressed == canvas->_point_right) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeRight);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (_hoveredElementGUI == canvas->_point_left_bottom || ElementGUI_pressed == canvas->_point_left_bottom) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottomLeft);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (_hoveredElementGUI == canvas->_point_bottom || ElementGUI_pressed == canvas->_point_bottom) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottom);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (_hoveredElementGUI == canvas->_point_right_bottom || ElementGUI_pressed == canvas->_point_right_bottom) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottomRight);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}
	
	
	if (dynamic_cast<LocationAndFilesSeparator*>(_hoveredElementGUI.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeHorizontal);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	if (dynamic_cast<PaletteValues*>(_hoveredElementGUI.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Cross);
		window->setMouseCursor(*_cursor);
		_ico = nullptr;
		_offset = sf::Vector2i(0, 0);
		_brushIsVisible = false;
		return;
	}

	sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
	if (lasso->_state == LassoState::Selected || lasso->_state == LassoState::Moving) {
		if (lasso->clickOnSelection(tile)) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeAll);
			window->setMouseCursor(*_cursor);
			_ico = nullptr;
			_offset = sf::Vector2i(0, 0);
			_brushIsVisible = false;
			return;
		}
	}

	if (_hoveredElementGUI == canvas) {

		if (canvas->_state == CanvasState::Moving) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeAll);
			window->setMouseCursor(*_cursor);
			_ico = nullptr;
			_offset = sf::Vector2i(0, 0);
			_brushIsVisible = false;
		}
		else if (toolbar->_toolType == ToolType::Brush) {
			window->setMouseCursorVisible(false);
			_ico = getTexture(L"tex\\cursor\\brush.png");
			_offset = sf::Vector2i(7, 7);
			_brushIsVisible = true;
		}
		else if (toolbar->_toolType == ToolType::Eraser) {
			window->setMouseCursorVisible(false);
			_ico = getTexture(L"tex\\cursor\\brush.png");
			_offset = sf::Vector2i(7, 7);
			_brushIsVisible = true;
		}
		else if (toolbar->_toolType == ToolType::Fill) {
			window->setMouseCursorVisible(true);
			_cursor = _fillCursor;
			window->setMouseCursor(*_cursor);
			_ico = nullptr;
			_offset = sf::Vector2i(0, 0);
			_brushIsVisible = false;
		}
		else if (toolbar->_toolType == ToolType::Picker) {
			window->setMouseCursorVisible(true);
			_cursor = _pickerCursor;
			window->setMouseCursor(*_cursor);
			_ico = nullptr;
			_offset = sf::Vector2i(0, 0);
			_brushIsVisible = false;
		}
		else {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Cross);
			window->setMouseCursor(*_cursor);
			_offset = sf::Vector2i(0, 0);
			_ico = nullptr;
			_brushIsVisible = false;
		}

		return;
	}

	window->setMouseCursorVisible(true);
	_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Arrow);
	window->setMouseCursor(*_cursor);
	_ico = nullptr;
	_offset = sf::Vector2i(0, 0);
	_brushIsVisible = false;
}

void Cursor::draw() {

	if (_mousePosition.x < 0 || _mousePosition.y < 0 || _mousePosition.x >= window->getSize().x || _mousePosition.y >= window->getSize().y) {
		return;
	}

	if (_brushIsVisible == true) {
		brush->draw(canvas->_position, canvas->_size, canvas->_zoom, canvas->_zoom_delta);
	}

	if (_ico != nullptr) {
		sf::Sprite sprite(*_ico->_texture);
		sprite.setOrigin(sf::Vector2f(_offset));
		
		if (_hoveredElementGUI == canvas) {
			sf::Vector2i p = worldToTile(_worldMousePosition, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
			sf::Vector2f p2 = sf::Vector2f(canvas->_position) + (sf::Vector2f(p) + sf::Vector2f(0.5f, 0.5f)) * canvas->_zoom * canvas->_zoom_delta;
			sprite.setPosition(p2);
		}
		else {
			sprite.setPosition(sf::Vector2f(_worldMousePosition));
		}

		window->draw(sprite);
	}
}

Cursor* cursor = nullptr;