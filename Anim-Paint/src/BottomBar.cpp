#include "BottomBar.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Canvas.hpp"
#include "WorldToTileConverter.hpp"
#include "Cursor.hpp"

BottomBar::BottomBar() : ElementGUI() {
	
	sf::Vector2i rectPosition;
	rectPosition.x = 0;
	rectPosition.y = (int)(mainView.getSize().y) - menu_height;

	sf::Vector2i rectSize;
	rectSize.x = 256 + 128;
	rectSize.y = menu_height;

	_rect = sf::IntRect(rectPosition, rectSize);
	
	_textCursorPosition = std::make_unique<sf::Text>(basicFont, L"0 x 0", menu_font_size);
	_textCursorPosition->setPosition(sf::Vector2f(32, mainView.getSize().y - menu_height + (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
	_textCursorPosition->setFillColor(normal_text_color);

	std::wstring csize = std::to_wstring(canvas->_size.x) + L" x " + std::to_wstring(canvas->_size.y);
	_textCanvasSize = std::make_unique<sf::Text>(basicFont, csize, menu_font_size);
	_textCanvasSize->setPosition(sf::Vector2f(128 +32, mainView.getSize().y - menu_height + (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
	_textCanvasSize->setFillColor(normal_text_color);

	std::wstring ssize = L"";
	_textSelectionSize = std::make_unique<sf::Text>(basicFont, ssize, menu_font_size);
	_textSelectionSize->setPosition(sf::Vector2f(256 + 32, mainView.getSize().y - menu_height + (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
	_textSelectionSize->setFillColor(normal_text_color);
}

BottomBar::~BottomBar() {

}

sf::Vector2i BottomBar::getSize() {
	return _rect.size;
}

void BottomBar::resize() {
	_rect.position = sf::Vector2i(0, (int)(mainView.getSize().y) - _rect.size.y);

	sf::Vector2f textCursorPositionPosition;
	textCursorPositionPosition.x = 32;
	textCursorPositionPosition.y = mainView.getSize().y - menu_height + (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2;
	_textCursorPosition->setPosition(textCursorPositionPosition);

	sf::Vector2f textCanvasSizePosition;
	textCanvasSizePosition.x = 128 + 32;
	textCanvasSizePosition.y = mainView.getSize().y - menu_height + (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2;
	_textCanvasSize->setPosition(textCanvasSizePosition);

	sf::Vector2f _textSelectionSizePosition;
	_textSelectionSizePosition.x = 256 + 32;
	_textSelectionSizePosition.y = mainView.getSize().y - menu_height + (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2;
	_textSelectionSize->setPosition(_textSelectionSizePosition);
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

	if (canvas->_state == CanvasState::Resizing) {
		_textCursorPosition->setString(L"");
		_textCanvasSize->setString(std::to_wstring(canvas->_size.x) + L" x " + std::to_wstring(canvas->_size.y));
	}

	if (lasso->_state != LassoState::None) {
		std::wstring ssize = std::to_wstring(lasso->_rect.size.x) + L" x " + std::to_wstring(lasso->_rect.size.y);
		_textSelectionSize->setString(ssize);
	}
	else {
		_textSelectionSize->setString(L"");
	}

}

void BottomBar::update() {

}

void BottomBar::draw() {
	
	sf::Vector2f rectSize;
	rectSize.x = (float)(_rect.size.x);
	rectSize.y = (float)(_rect.size.y);

	sf::Vector2f rectPosition;
	rectPosition.x = (float)(_rect.position.x);
	rectPosition.y = (float)(_rect.position.y);

	sf::RectangleShape rect(rectSize);
	rect.setPosition(rectPosition);
	rect.setFillColor(tools_bar_color);
	window->draw(rect);

	sf::Sprite cursorPositionIcon(*getTexture(L"tex\\bottom_bar\\cursor_position.png")->_texture);
	cursorPositionIcon.setPosition(sf::Vector2f(0, mainView.getSize().y - menu_height + (menu_height - cursorPositionIcon.getGlobalBounds().size.y) / 2));
	window->draw(cursorPositionIcon);

	window->draw(*_textCursorPosition);

	sf::Sprite canvasSizeIcon(*getTexture(L"tex\\bottom_bar\\canvas_size.png")->_texture);
	canvasSizeIcon.setPosition(sf::Vector2f(128, mainView.getSize().y - menu_height + (menu_height - canvasSizeIcon.getGlobalBounds().size.y) / 2));
	window->draw(canvasSizeIcon);
	window->draw(*_textCanvasSize);

	sf::Sprite selectionSizeIcon(*getTexture(L"tex\\bottom_bar\\selection_size.png")->_texture);
	selectionSizeIcon.setPosition(sf::Vector2f(256, mainView.getSize().y - menu_height + (menu_height - selectionSizeIcon.getGlobalBounds().size.y) / 2));
	window->draw(selectionSizeIcon);
	window->draw(*_textSelectionSize);


}

std::shared_ptr<BottomBar> bottom_bar;