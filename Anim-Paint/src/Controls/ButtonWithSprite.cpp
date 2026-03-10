#include "Controls/ButtonWithSprite.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

ButtonWithSprite::ButtonWithSprite(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position)
: Button() {

	_texture = texture;
	_hoverTexture = hoverTexture;

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

sf::Vector2i ButtonWithSprite::getSize() {
	return _rect.size;
}

void ButtonWithSprite::setPosition(sf::Vector2i position) {
	_rect.position = position;
}

void ButtonWithSprite::unclick() {
	_state = ButtonState::Idle;
}

void ButtonWithSprite::hover() {
	_state = ButtonState::Hover;
}

void ButtonWithSprite::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}

void ButtonWithSprite::cursorHover() {
	Button::cursorHover();

}

void ButtonWithSprite::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_position)) {
		
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left)	{		
			Element_pressed = this->shared_from_this();
			return;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (Element_pressed.get() == this) {
				click();
				return;
			}
		}
	}

	if (_isSelected && _activatedByEnter) {
		if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Enter) {
			click();
			return;
		}
	}
}

void ButtonWithSprite::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}

			if (Element_pressed.get() == this)
				Element_pressed = nullptr;
			unclick();
		}
	}
	else if (Element_hovered.get() == this) {
		hover();
	}
	else
		unclick();
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

	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

}