#include "Controls/ColoredButtonWithText.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"

ColoredButtonWithText::ColoredButtonWithText(std::wstring text, sf::Vector2i size, sf::Vector2i position) : Button()
 {

	_text = std::make_unique<sf::Text>(basicFont, text, 13);
	_text->setFillColor(dark_and_red_button_text_color);

	_rectIdleColor = dark_and_red_button_idle_color;
	_rectHoverColor = dark_and_red_button_hover_color;
	_rectPressColor = dark_and_red_button_press_color;
	_rectSelectColor = dark_and_red_button_select_color;

	_rectBorderWidth = dialog_border_width;

	_rectIdleBorderColor = dialog_border_color;
	_rectHoverBorderColor = dialog_border_color;
	_rectPressBorderColor = dialog_border_color;
	_rectSelectBorderColor = dialog_border_color;

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(size));

	// zbędne
	// _text = nullptr;

	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

ColoredButtonWithText::~ColoredButtonWithText() {
	
}

sf::Vector2i ColoredButtonWithText::getSize() {
	return _rect.size;
}

void ColoredButtonWithText::setPosition(sf::Vector2i position) {
	_rect.position = sf::Vector2i(position);

	sf::Vector2f text_pos;
	text_pos.x = _rect.position.x + _rect.size.x / 2 - _text->getGlobalBounds().size.x / 2;
	text_pos.y = _rect.position.y + _rect.size.y / 2 - basicFont.getLineSpacing(13) / 2;
	_text->setPosition(text_pos);
	
}

void ColoredButtonWithText::unclick() {
	_state = ButtonState::Idle;
}

void ColoredButtonWithText::hover() {
	_state = ButtonState::Hover;
}

void ColoredButtonWithText::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}


void ColoredButtonWithText::cursorHover() {
	Button::cursorHover();
}

void ColoredButtonWithText::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_position)) {
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
			Element_pressed = this->shared_from_this();
			click();
			return;
		}
	}
}

void ColoredButtonWithText::update() {

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

void ColoredButtonWithText::draw() {

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

	window->draw(*_text);
}
