#include "Controls/ButtonWithSprite.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

ButtonWithSprite::ButtonWithSprite(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, std::shared_ptr<Texture> pressTexture, sf::Vector2i position)
: Button() {

	_texture = texture;
	_hoverTexture = hoverTexture;
	_pressTexture = pressTexture;

	sf::Vector2i rectSize = sf::Vector2i(_texture->_texture->getSize());
	_rect = sf::IntRect(sf::Vector2i(0, 0), rectSize);

	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

ButtonWithSprite::~ButtonWithSprite() {
	
}

void ButtonWithSprite::draw() {

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x - 2 * _rectBorderWidth);
	rectSize.y = float(_rect.size.y - 2 * _rectBorderWidth);

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

	std::shared_ptr<Texture> texture;
	switch (_state)
	{
	case ButtonState::Idle:
		texture = _texture;
		break;
	case ButtonState::Hover:	
		texture = _hoverTexture;
		break;
	case ButtonState::Pressed:
		texture = _pressTexture;
		break;
	default:
		texture = _texture;
		break;
	}

	sf::Sprite sprite(*texture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

}