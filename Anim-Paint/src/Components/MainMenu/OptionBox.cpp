#include "Components/MainMenu/OptionBox.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

OptionBox::OptionBox(std::wstring text, std::wstring shortcut) : Button() {
	_text = std::make_unique<sf::Text>(basicFont, text, menu_font_size);
	_text->setFillColor(menu_text_color);

	_shortcut_text = std::make_unique<sf::Text>(basicFont, shortcut, menu_font_size);
	_shortcut_text->setFillColor(menu_text_color);

	_rect.size.x = optionbox_left_margin + optionbox_right_margin + (int)_text->getGlobalBounds().size.x + (2 * menu_horizontal_margin);
	if(shortcut != L"")
		_rect.size.x += (int)(_shortcut_text->getGlobalBounds().size.x) + menu_horizontal_margin;
	_rect.size.y = menu_height;

	_rectIdleColor = optionbox_idle_color;
	_rectHoverColor = optionbox_hover_color;
	_rectPressColor = optionbox_press_color;
	_rectSelectColor = optionbox_select_color;

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

	setPosition(_rect.position, _rect.size);
}

OptionBox::~OptionBox() {

}

void OptionBox::setPosition(sf::Vector2i position, sf::Vector2i size) {
	_rect.position = position;

	sf::Vector2f textPos;
	textPos.x = (float)(position.x + menu_horizontal_margin + optionbox_left_margin);
	textPos.y = (float)(position.y) + ((float)menu_height - basicFont.getLineSpacing(menu_font_size)) / 2.0f;
	_text->setPosition(textPos);

	sf::Vector2f shortcutTextPos;
	shortcutTextPos.x = (float)(position.x + size.x - menu_horizontal_margin - optionbox_right_margin - _shortcut_text->getGlobalBounds().size.x);
	shortcutTextPos.y = (float)(position.y) + ((float)menu_height - basicFont.getLineSpacing(menu_font_size)) / 2.0f;
	_shortcut_text->setPosition(shortcutTextPos);

	
}

void OptionBox::draw() {

	// draw rectangle
	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x - 2 * _rectBorderWidth);
	rectSize.y = float(_rect.size.y - 2 * _rectBorderWidth);
	sf::RectangleShape rect(rectSize);
	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(_rectPressColor);
		break;
	case ButtonState::Hover:
		rect.setFillColor(_rectHoverColor);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(_rectSelectColor);
		}
		else {
			rect.setFillColor(_rectIdleColor);
		};
		break;
	};
	sf::Vector2f rectPosition;
	rectPosition.x = float(_rect.position.x + _rectBorderWidth);
	rectPosition.y = float(_rect.position.y + _rectBorderWidth);
	rect.setPosition(rectPosition);
	window->draw(rect);

	window->draw(*_text);
	window->draw(*_shortcut_text);
}