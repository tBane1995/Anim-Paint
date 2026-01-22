#include "BottomBar.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Canvas.hpp"
#include "WorldToTileConverter.hpp"
#include "Cursor.hpp"

BottomBar::BottomBar() : ElementGUI() {
	_rect = sf::IntRect(sf::Vector2i(0,mainView.getSize().y - menu_height), sf::Vector2i(256, menu_height));
	
	_textCursorPosition = std::make_unique<sf::Text>(basicFont, L"0 x 0", menu_font_size);
	_textCursorPosition->setPosition(sf::Vector2f(32, mainView.getSize().y - menu_height + (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
	_textCursorPosition->setFillColor(normal_text_color);

	std::wstring csize = std::to_wstring(canvas->_size.x) + L" x " + std::to_wstring(canvas->_size.y);
	_textCanvasSize = std::make_unique<sf::Text>(basicFont, csize, menu_font_size);
	_textCanvasSize->setPosition(sf::Vector2f(160, mainView.getSize().y - menu_height + (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
	_textCanvasSize->setFillColor(normal_text_color);
}

BottomBar::~BottomBar() {

}

void BottomBar::cursorHover() {

}

void BottomBar::handleEvent(const sf::Event& event) {

	if (ElementGUI_hovered != canvas && ElementGUI_hovered != canvas->_hoveredEdgePoint && _hoveredElementGUI == ElementGUI_hovered)
		return;

	_hoveredElementGUI = ElementGUI_hovered;

	if (_hoveredElementGUI == canvas) {
		sf::Vector2i coords = worldToTile(cursor->_worldMousePosition, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
		std::wstring text = std::to_wstring(coords.x) + L" x " + std::to_wstring(coords.y);
		_textCursorPosition->setString(text);
	}

	if (_hoveredElementGUI == nullptr) {
		_textCursorPosition->setString(L"");
		_textCanvasSize->setString(std::to_wstring(canvas->_size.x) + L" x " + std::to_wstring(canvas->_size.y));
	}

	if (canvas->_clickedEdgePoint != nullptr) {
		_textCursorPosition->setString(L"");
		_textCanvasSize->setString(std::to_wstring(canvas->_size.x) + L" x " + std::to_wstring(canvas->_size.y));
	}

}

void BottomBar::update() {

}

void BottomBar::draw() {
	
	sf::RectangleShape rect(sf::Vector2f(_rect.size.x, _rect.size.y));
	rect.setPosition(sf::Vector2f(_rect.position.x, _rect.position.y));
	rect.setFillColor(tools_bar_color);
	window->draw(rect);

	window->draw(*_textCursorPosition);
	window->draw(*_textCanvasSize);


}

std::shared_ptr<BottomBar> bottom_bar;