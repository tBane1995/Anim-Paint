#include "Components/Toolbar/Option.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

Option::Option(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position) : Button() {

	_texture = texture;
	_hoverTexture = hoverTexture;

	_rectIdleColor = optionbox_idle_color;
	_rectHoverColor = optionbox_hover_color;
	_rectPressColor = optionbox_press_color;
	_rectSelectColor = optionbox_select_color;

	_text = std::make_unique<sf::Text>(basicFont, text, 13);
	_text->setFillColor(menu_text_color);

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32 + (int)_text->getGlobalBounds().size.x + 8, 32));

	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

Option::~Option() {
}

sf::Vector2i Option::getSize() {
	return _rect.size;
}

void Option::setPosition(sf::Vector2i position) {
	_rect.position = sf::Vector2i(position);
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(32, (32 - basicFont.getLineSpacing(13)) / 2 - 1));

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

	// Draw sprite
	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

	// draw text
	window->draw(*_text);
}