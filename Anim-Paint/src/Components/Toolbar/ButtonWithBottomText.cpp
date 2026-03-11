#include "Components/Toolbar/ButtonWithBottomText.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

ButtonWithBottomText::ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position) : Button()
{
	_rectIdleColor = rectColor;
	_rectHoverColor = rectColor;
	_rectPressColor = rectColor;
	_rectSelectColor = rectColor;

	_rectBorderWidth = tools_border_width;
	_rectIdleBorderColor = tools_button_idle_border_color;
	_rectHoverBorderColor = tools_button_hover_border_color;
	_rectPressBorderColor = tools_button_press_border_color;
	_rectSelectBorderColor = tools_button_select_border_color;

	_textColor = textColor;
	_hoverTextColor = hoverTextColor;

	_text = std::make_unique<sf::Text>(basicFont, text, 13);
	_text->setFillColor(_textColor);


	_texture = texture;
	_hoverTexture = hoverTexture;

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(48, 64));

	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

ButtonWithBottomText::~ButtonWithBottomText() {

}

sf::Vector2i ButtonWithBottomText::getSize() {
	return _rect.size;
}

void ButtonWithBottomText::setPosition(sf::Vector2i position) {
	_rect.position = position;
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(48 / 2 - _text->getGlobalBounds().size.x / 2.0f, _rect.size.y - basicFont.getLineSpacing(13) - 4));

}

void ButtonWithBottomText::unclick() {
	_state = ButtonState::Idle;
	_text->setFillColor(_textColor);
}

void ButtonWithBottomText::hover() {
	_state = ButtonState::Hover;
	_text->setFillColor(_hoverTextColor);
}

void ButtonWithBottomText::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
	_text->setFillColor(_hoverTextColor);
}

void ButtonWithBottomText::draw() {

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x - 2 * tools_border_width);
	rectSize.y = float(_rect.size.y - 2 * tools_border_width);

	sf::RectangleShape rect(rectSize);

	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(_rectPressColor);
		rect.setOutlineThickness((float)_rectBorderWidth);
		rect.setOutlineColor(_rectPressBorderColor);
		break;
	case ButtonState::Hover:
		rect.setFillColor(_rectHoverColor);
		rect.setOutlineThickness((float)_rectBorderWidth);
		rect.setOutlineColor(_rectHoverBorderColor);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(_rectSelectColor);
			rect.setOutlineThickness((float)_rectBorderWidth);
			rect.setOutlineColor(_rectSelectBorderColor);
		}
		else {
			rect.setFillColor(_rectIdleColor);
			rect.setOutlineThickness((float)_rectBorderWidth);
			rect.setOutlineColor(_rectIdleBorderColor);
		};
		break;
	};

	sf::Vector2f rectPosition;
	rectPosition.x = float(_rect.position.x + _rectBorderWidth);
	rectPosition.y = float(_rect.position.y + _rectBorderWidth);
	rect.setPosition(rectPosition);
	window->draw(rect);

	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

	window->draw(*_text);
}