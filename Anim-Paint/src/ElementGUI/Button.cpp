#include "ElementGUI/Button.hpp"
#include "SFML/Graphics.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"
#include <iostream>
#include <memory>

//////////////////////////////////////////////////////////////////
// Button

Button::Button() : ElementGUI() {
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

void Button::select() {
	_isSelected = true;
}

void Button::unselect() {
	_isSelected = false;
}

void Button::activateByEnter(bool activate) {
	_activatedByEnter = activate;
}

void Button::setPosition(sf::Vector2i position) {
	_rect.position = position;
}

void Button::cursorHover() {

}

void Button::handleEvent(const sf::Event& event) {

}

void Button::update() {

}

void Button::draw() {

}

//////////////////////////////////////////////////////////////////
// NormalButton

NormalButton::NormalButton(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position)
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

NormalButton::~NormalButton() {
	
}

sf::Vector2i NormalButton::getSize() {
	return _rect.size;
}

void NormalButton::setPosition(sf::Vector2i position) {
	_rect.position = position;
}

void NormalButton::unclick() {
	_state = ButtonState::Idle;
}

void NormalButton::hover() {
	_state = ButtonState::Hover;
}

void NormalButton::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}

void NormalButton::cursorHover() {

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}


}

void NormalButton::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left)	{		
			ElementGUI_pressed = this->shared_from_this();
			return;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed.get() == this) {
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

void NormalButton::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();
}

void NormalButton::draw() {

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

//////////////////////////////////////////////////////////////////
// NormalButtonWithText

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

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}


}

void ColoredButtonWithText::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
			return;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed.get() == this) {
				click();
				return;
			}
		}
	}

	if (_isSelected && _activatedByEnter) {
		if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Enter) {
			ElementGUI_pressed = this->shared_from_this();
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
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered.get() == this) {
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

//////////////////////////////////////////////////////////////////
// ButtonWithBottomText

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


void ButtonWithBottomText::cursorHover() {

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}


}

void ButtonWithBottomText::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed.get() == this) {
				click();
			}
		}

	}
}

void ButtonWithBottomText::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();
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

//////////////////////////////////////////////////////////////////
// ButtonWithRightText

ButtonWithRightText::ButtonWithRightText(std::wstring text, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position) : Button()
{
	_rectIdleColor = tools_button_idle_color;
	_rectHoverColor = tools_button_hover_color;
	_rectPressColor = tools_button_press_color;
	_rectSelectColor = tools_button_select_color;

	_rectBorderWidth = tools_border_width;
	_rectIdleBorderColor = tools_button_idle_border_color;
	_rectHoverBorderColor = tools_button_hover_border_color;
	_rectPressBorderColor = tools_button_press_border_color;
	_rectSelectBorderColor = tools_button_select_border_color;

	_textColor = textColor;
	_hoverTextColor = hoverTextColor;

	_texture = texture;
	_hoverTexture = hoverTexture;

	_text = std::make_unique<sf::Text>(basicFont, text, 13);
	_text->setFillColor(_textColor);

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(64, 32));

	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

ButtonWithRightText::~ButtonWithRightText() {

}

sf::Vector2i ButtonWithRightText::getSize() {
	return _rect.size;
}

void ButtonWithRightText::setPosition(sf::Vector2i position) {
	_rect.position = position;
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(32, 24 - basicFont.getLineSpacing(13)));
}

void ButtonWithRightText::unclick() {
	_state = ButtonState::Idle;
	_text->setFillColor(_textColor);
}

void ButtonWithRightText::hover() {
	_state = ButtonState::Hover;
	_text->setFillColor(_hoverTextColor);
}

void ButtonWithRightText::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
	_text->setFillColor(_hoverTextColor);
}


void ButtonWithRightText::cursorHover() {

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}


}

void ButtonWithRightText::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed.get() == this) {
				click();
			}
		}

	}
}

void ButtonWithRightText::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();
}

void ButtonWithRightText::draw() {

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

	window->draw(*_text);
}

//////////////////////////////////////////////////////////////////
// Option

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
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(32, (32 - basicFont.getLineSpacing(13))/2-1));

}

void Option::unclick() {
	_state = ButtonState::Idle;
}

void Option::hover() {
	_state = ButtonState::Hover;
}

void Option::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}


void Option::cursorHover() {

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}


}

void Option::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed.get() == this) {
				click();
			}
		}

	}
}

void Option::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();
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

//////////////////////////////////////////////////////////////////
// ButtonWithTopTextAndList

ButtonWithTopTextAndList::ButtonWithTopTextAndList(std::wstring text, sf::Color textColor, sf::Color hoverTextColor, sf::Vector2i position) : Button() {

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(48, 32));

	_rectIdleColor = tools_button_idle_color;
	_rectHoverColor = tools_button_hover_color;
	_rectPressColor = tools_button_press_color;
	_rectSelectColor = tools_button_select_color;
		
	_rectBorderWidth = tools_border_width;

	_rectIdleBorderColor = tools_button_idle_border_color;
	_rectHoverBorderColor = tools_button_hover_border_color;
	_rectPressBorderColor = tools_button_press_border_color;
	_rectSelectBorderColor = tools_button_select_border_color;

	_texture = getTexture(L"tex\\tools\\bottom_arrow.png");
	_hoverTexture = getTexture(L"tex\\tools\\bottom_arrow_hover.png");

	_textColor = textColor;
	_hoverTextColor = hoverTextColor;
	
	_text = std::make_unique<sf::Text>(basicFont, text, 13);
	_text->setFillColor(_textColor);

	setPosition(position);

	_list_rect = sf::RectangleShape();
	_list_rect.setFillColor(menuoptions_border_color);
	_options.clear();
	_isOpen = false;

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

	addOption(L"paste");
	addOption(L"from file");
	addOption(L"transparency");
}

ButtonWithTopTextAndList::~ButtonWithTopTextAndList() { 

}

sf::Vector2i ButtonWithTopTextAndList::getSize() {
	return _rect.size;
}

void ButtonWithTopTextAndList::addOption(std::wstring text) {
	std::shared_ptr<Option> o = std::make_shared<Option>(text, getTexture(L"tex\\tools\\btn_none.png"), getTexture(L"tex\\tools\\btn_none_hover.png"));

	if (_options.size() == 0) {
		_options.push_back(o);
		sf::Vector2f rectSize;
		rectSize.x = 0;
		rectSize.y = float(o->_rect.size.y + menuoptions_border_width);
		_list_rect.setSize(rectSize);
		return;
	}

	int wdt = (_options.back()->_rect.size.x > o->_rect.size.x) ? _options.back()->_rect.size.x : o->_rect.size.x;
	sf::Vector2f rectSize;
	rectSize.x = float(wdt + 2 * menuoptions_border_width);
	rectSize.y = float(_list_rect.getSize().y + o->_rect.size.y);
	_list_rect.setSize(rectSize);

	_options.push_back(o);
	for (auto& o : _options) {
		o->_rect.size = sf::Vector2i(wdt, 32);
	}
}

void ButtonWithTopTextAndList::setPosition(sf::Vector2i position) {
	
	_rect.position = position;

	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(48 / 2 - _text->getGlobalBounds().size.x / 2.0f, 0));

	sf::Vector2i ipos = _rect.position + sf::Vector2i(0, _rect.size.y);

	sf::Vector2f rectPos;
	rectPos.x = float(ipos.x - menuoptions_border_width);
	rectPos.y = float(ipos.y);

	_list_rect.setPosition(rectPos);
	for (auto& o : _options) {
		o->setPosition(ipos);
		ipos.y += 32;
	}
}

void ButtonWithTopTextAndList::unclick() {
	_state = ButtonState::Idle;
	_text->setFillColor(_textColor);
	
}

void ButtonWithTopTextAndList::hover() {
	_state = ButtonState::Hover;
	_text->setFillColor(_hoverTextColor);
}

void ButtonWithTopTextAndList::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
	_text->setFillColor(_hoverTextColor);
}


void ButtonWithTopTextAndList::cursorHover() {

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}

	if (_isOpen) {
		for (auto& option : _options)
			option->cursorHover();
	}


}

void ButtonWithTopTextAndList::handleEvent(const sf::Event& event) {

	if (_rect.contains(cursor->_worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed.get() == this) {
				click();
				_isOpen = !_isOpen;
			}
		}
	}

	bool clicked_in_menu = false;

	if (_isOpen) {
		for (auto& option : _options) {
			option->handleEvent(event);
			if (ElementGUI_pressed == option)
				clicked_in_menu = true;
		}
	}

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (clicked_in_menu == false) {
			_isOpen = false;
		}
	}

}

void ButtonWithTopTextAndList::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();


	for (auto& option : _options)
		option->update();
}

void ButtonWithTopTextAndList::draw() {

	// draw rect
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

	if (_isOpen) {
		rect.setFillColor(_rectPressColor);
		rect.setOutlineThickness((float)_rectBorderWidth);
		rect.setOutlineColor(_rectPressBorderColor);
	}

	sf::Vector2f rectPosition;
	rectPosition.x = float(_rect.position.x + _rectBorderWidth);
	rectPosition.y = float(_rect.position.y + _rectBorderWidth);
	rect.setPosition(rectPosition);
	window->draw(rect);

	// draw sprite
	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(rectPosition + sf::Vector2f(0,16.0f));
	window->draw(sprite);

	// draw text
	window->draw(*_text);

	if (_isOpen) {

		window->draw(_list_rect);
		for (auto& o : _options) {
			o->draw();
		}

	}
}