 #include "Cursor.hpp"
#include "SFML/Graphics.hpp"
#include "Dialogs/FileDialog/FileDialog.hpp"
#include "Components/Canvas.hpp"
#include "Tools/Brush.hpp"
#include "Dialogs/Palette.hpp"
#include "Tools/Selection.hpp"
#include "Controls/Slider.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Time.hpp" // TO-DO - to remove
#include <typeinfo> // for class name

Cursor::Cursor() {

	_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Arrow);
	_position = sf::Mouse::getPosition(*window);
	window->setMouseCursor(*_cursor);
	_hoveredElement = nullptr;
	_brushIsVisible = false;

	// cursors
	

	{
		sf::Image img = getTexture(L"tex\\cursor\\brush.png")->_texture->copyToImage();
		_brushCursor = std::make_shared<sf::Cursor>(img.getPixelsPtr(), img.getSize(), sf::Vector2u(7, 7));
	}

	{
		sf::Image img = getTexture(L"tex\\cursor\\fill.png")->_texture->copyToImage();
		_fillCursor = std::make_shared<sf::Cursor>(img.getPixelsPtr(), img.getSize(), sf::Vector2u(6, 26));
	}

	{
		sf::Image img = getTexture(L"tex\\cursor\\picker.png")->_texture->copyToImage();
		_pickerCursor = std::make_shared<sf::Cursor>(img.getPixelsPtr(), img.getSize(), sf::Vector2u(5, 26));
	}

	{
		sf::Image img = getTexture(L"tex\\cursor\\cross.png")->_texture->copyToImage();
		_crossCursor = std::make_shared<sf::Cursor>(img.getPixelsPtr(), img.getSize(), sf::Vector2u(11, 11));
	}
}

Cursor::~Cursor() {

}

void Cursor::update() {
	_position = sf::Mouse::getPosition(*window);
}

void Cursor::handleEvent() {

	brush->setPosition(worldToTile(_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta));

	std::shared_ptr<TextInput> input = dynamic_pointer_cast<TextInput>(Element_pressed);
	if (input != nullptr && input->_editState == TextInputEditState::Selecting) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Text);
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	if(main_menu->_state != MainMenuStates::Closed)
		return;

	_hoveredElement = Element_hovered;
	
	/*
	if (_hoveredElement) {
		std::string className = typeid(*_hoveredElement).name();		// get class name
		std::wstring wClassName(className.begin(), className.end());	// convert to wide_string
		std::wcout << wClassName + L"\n";
	}
	else {
		//std::wcout << L"nullptr\n";
	}
	*/
	
	if (dynamic_cast<TextInput*>(_hoveredElement.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Text);
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	if (dynamic_cast<Slider*>(_hoveredElement.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Hand);
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	if (dynamic_cast<Button*>(_hoveredElement.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Arrow);
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	// resizable_tool edge points
	if ((resizable_tool != nullptr &&
		((resizable_tool->_hoveredEdgePoint != nullptr && _hoveredElement == resizable_tool->_hoveredEdgePoint) || 
		(resizable_tool->_clickedEdgePoint != nullptr && Element_pressed == resizable_tool->_clickedEdgePoint))) &&
		!(palette != nullptr && palette->_rect.contains(_position)) &&
		(resizable_tool->_state == ResizableToolState::Selected || resizable_tool->_state == ResizableToolState::Resizing)) {

		if (resizable_tool->_hoveredEdgePoint == resizable_tool->_point_left_top || resizable_tool->_clickedEdgePoint == resizable_tool->_point_left_top) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTopLeft);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (resizable_tool->_hoveredEdgePoint == resizable_tool->_point_top || resizable_tool->_clickedEdgePoint == resizable_tool->_point_top) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTop);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (resizable_tool->_hoveredEdgePoint == resizable_tool->_point_right_top || resizable_tool->_clickedEdgePoint == resizable_tool->_point_right_top) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTopRight);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (resizable_tool->_hoveredEdgePoint == resizable_tool->_point_left || resizable_tool->_clickedEdgePoint == resizable_tool->_point_left) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeLeft);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (resizable_tool->_hoveredEdgePoint == resizable_tool->_point_right || resizable_tool->_clickedEdgePoint == resizable_tool->_point_right) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeRight);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (resizable_tool->_hoveredEdgePoint == resizable_tool->_point_left_bottom || resizable_tool->_clickedEdgePoint == resizable_tool->_point_left_bottom) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottomLeft);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (resizable_tool->_hoveredEdgePoint == resizable_tool->_point_bottom || resizable_tool->_clickedEdgePoint == resizable_tool->_point_bottom) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottom);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (resizable_tool->_hoveredEdgePoint == resizable_tool->_point_right_bottom || resizable_tool->_clickedEdgePoint == resizable_tool->_point_right_bottom) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottomRight);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}
	}

	if (canvas->_state == CanvasState::Moving) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeAll);
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	// canvas edge points
	if (!(palette != nullptr && palette->_rect.contains(_position))) {
		if (_hoveredElement == canvas->_point_left_top || Element_pressed == canvas->_point_left_top) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTopLeft);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (_hoveredElement == canvas->_point_top || Element_pressed == canvas->_point_top) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTop);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (_hoveredElement == canvas->_point_right_top || Element_pressed == canvas->_point_right_top) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeTopRight);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (_hoveredElement == canvas->_point_left || Element_pressed == canvas->_point_left) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeLeft);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (_hoveredElement == canvas->_point_right || Element_pressed == canvas->_point_right) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeRight);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (_hoveredElement == canvas->_point_left_bottom || Element_pressed == canvas->_point_left_bottom) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottomLeft);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (_hoveredElement == canvas->_point_bottom || Element_pressed == canvas->_point_bottom) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottom);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}

		if (_hoveredElement == canvas->_point_right_bottom || Element_pressed == canvas->_point_right_bottom) {
			window->setMouseCursorVisible(true);
			_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeBottomRight);
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
			return;
		}
	}

	if (resizable_tool != nullptr && Element_pressed == resizable_tool && resizable_tool->_state == ResizableToolState::Selecting) {
		window->setMouseCursorVisible(true);
		_cursor = _crossCursor;
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
	if ((_hoveredElement == resizable_tool && resizable_tool != nullptr && resizable_tool->_state == ResizableToolState::Selected && !(palette!=nullptr && palette->_rect.contains(_position))) ||
		(_hoveredElement == resizable_tool && resizable_tool != nullptr && resizable_tool->_state == ResizableToolState::Moving)) {

		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeAll);
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	if (dynamic_cast<LocationAndFilesSeparator*>(_hoveredElement.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::SizeHorizontal);
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	if (dynamic_cast<PaletteValues*>(_hoveredElement.get()) != nullptr) {
		window->setMouseCursorVisible(true);
		_cursor = _crossCursor;
		window->setMouseCursor(*_cursor);
		_brushIsVisible = false;
		return;
	}

	bool hoveredCanvas = false;
	for (auto& canvas : canvases) {
		if(_hoveredElement == canvas) {
			hoveredCanvas = true;
			break;
		}
	}

	if (hoveredCanvas) {
		if (toolbar->_toolType == ToolType::Brush) {
			window->setMouseCursorVisible(true);
			_cursor = _brushCursor;
			window->setMouseCursor(*_cursor);
			_brushIsVisible = true;
		}
		else if (toolbar->_toolType == ToolType::Eraser) {
			window->setMouseCursorVisible(true);
			_cursor = _brushCursor;
			window->setMouseCursor(*_cursor);
			_brushIsVisible = true; 
		}
		else if (toolbar->_toolType == ToolType::Fill) {
			window->setMouseCursorVisible(true);
			_cursor = _fillCursor;
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;	
		}
		else if (toolbar->_toolType == ToolType::Picker) {
			window->setMouseCursorVisible(true);
			_cursor = _pickerCursor;
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
		}
		else {
			window->setMouseCursorVisible(true);
			_cursor = _crossCursor;
			window->setMouseCursor(*_cursor);
			_brushIsVisible = false;
		}

		return;
	}

	

	int d = 4;
	if (_position.x <= d || _position.y <= d || _position.x >= int(window->getSize().x - d) || _position.y >= int(window->getSize().y - d)) {
		return;
	}

	window->setMouseCursorVisible(true);
	_cursor = std::make_shared<sf::Cursor>(sf::Cursor::Type::Arrow);
	window->setMouseCursor(*_cursor);
	_brushIsVisible = false;
}

void Cursor::draw() {

	int d = 4;
	if (_position.x <= d || _position.y <= d || _position.x >= int(window->getSize().x - d) || _position.y >= int(window->getSize().y - d)) {
		return;
	}

	if (_brushIsVisible == true) {
		brush->draw();
	}
}

std::shared_ptr<Cursor> cursor;