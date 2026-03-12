#include "Controls/Button.hpp"
#include "SFML/Graphics.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"
#include <iostream>
#include <memory>
#include "Components/Tooltip.hpp"

Button::Button() : Element() {
	_isSelected = false;

	_rectIdleColor = sf::Color::Transparent;
	_rectHoverColor = sf::Color::Transparent;
	_rectPressColor = sf::Color::Transparent;
	_rectSelectColor = sf::Color::Transparent;

	_rectBorderWidth = 0;
	_rectIdleBorderColor = sf::Color::Transparent;
	_rectHoverBorderColor = sf::Color::Transparent;
	_rectPressBorderColor = sf::Color::Transparent;
	_rectSelectBorderColor = sf::Color::Transparent;

	_activatedByEnter = false;

	_state = ButtonState::Idle;
	_hover_func = {};
	_onclick_func = {};
	_clickTime = sf::Time::Zero;

	_title = L"";
	_description = L"";
}



Button::~Button() {

}

void Button::setRectColors(sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor, sf::Color selectColor) {
	_rectIdleColor = idleColor;
	_rectHoverColor = hoverColor;
	_rectPressColor = pressColor;
	_rectSelectColor = selectColor;
}

void Button::setRectColors(sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor, sf::Color selectColor,
	int borderWidth, sf::Color idleBorderColor, sf::Color hoverBorderColor, sf::Color pressBorderColor, sf::Color selectBorderColor) {
	_rectIdleColor = idleColor;
	_rectHoverColor = hoverColor;
	_rectPressColor = pressColor;
	_rectSelectColor = selectColor;

	_rectBorderWidth = borderWidth;

	_rectIdleBorderColor = idleBorderColor;
	_rectHoverBorderColor = hoverBorderColor;
	_rectPressBorderColor = pressBorderColor;
	_rectSelectBorderColor = selectBorderColor;
}

sf::Vector2i Button::getSize() { 
	return _rect.size; 
}

sf::Vector2i Button::getPosition() {
	return _rect.position;
}

void Button::setPosition(sf::Vector2i position) {
	_rect.position = position;
}

void Button::select() {
	_isSelected = true;
}

void Button::unselect() {
	_isSelected = false;
}

void Button::unclick() {
	_state = ButtonState::Idle;
}

void Button::hover() {
	_state = ButtonState::Hover;
}

void Button::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}

void Button::activateByEnter(bool activate) {
	_activatedByEnter = activate;
}

void Button::setTooltip(std::wstring title, std::wstring description) {
	_title = title;
	_description = description;
}

void Button::cursorHover() {
	if (_rect.contains(cursor->_position)) {
		Element_hovered = this->shared_from_this();
		tooltip->setButton(std::dynamic_pointer_cast<Button>(this->shared_from_this()));
	}
}

void Button::handleEvent(const sf::Event& event) {
	if (Element_hovered.get() == this) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
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

void Button::update() {
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

void Button::draw() {
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
}