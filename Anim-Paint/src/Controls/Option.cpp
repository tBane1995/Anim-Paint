#include "Controls/Option.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

Option::Option(std::wstring text, std::wstring shortcut, sf::Vector2i position) : Button() {

	_rectIdleColor = optionbox_idle_color;
	_rectHoverColor = optionbox_hover_color;
	_rectPressColor = optionbox_press_color;
	_rectSelectColor = optionbox_select_color;

	_text = std::make_unique<sf::Text>(basicFont, text, optionbox_font_size);

	_shortcut_text = std::make_unique<sf::Text>(basicFont, shortcut, optionbox_font_size);

	sf::Vector2i size;
	size.y = optionbox_height;
	size.x = 32 + optionbox_left_margin + getTextWidth() + optionbox_right_margin;
	if(getShortcutTextWidth() > 0)
		size.x += optionbox_spacing + getShortcutTextWidth();

	_rect = sf::IntRect(sf::Vector2i(0, 0), size);

	setPosition(position, getTextWidth());

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

Option::~Option() {
}

void Option::setPosition(sf::Vector2i position, int shortcut_offset) {
	_rect.position = position;
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(32, (_rect.size.y - basicFont.getLineSpacing(optionbox_font_size)) / 2 - 1));
	_shortcut_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(32 + shortcut_offset, (_rect.size.y - basicFont.getLineSpacing(optionbox_font_size)) / 2 - 1));

}

int Option::getTextWidth() {
	if (_text == nullptr) return 0;
	return (int)(_text->getGlobalBounds().size.x);
}

int Option::getShortcutTextWidth() {
	if (_shortcut_text == nullptr) return 0;
	return (int)(_shortcut_text->getGlobalBounds().size.x);
}

void Option::draw() {

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

	// draw text
	sf::Color textColor = (_isActive) ? menu_text_color : menu_text_inactive_color;

	_text->setFillColor(textColor);
	_shortcut_text->setFillColor(textColor);

	window->draw(*_text);
	window->draw(*_shortcut_text);
}
