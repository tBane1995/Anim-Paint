﻿#include "ElementGUI/Button.hpp"
#include "SFML/Graphics.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Mouse.hpp"
#include "Window.hpp"
#include <iostream>

//////////////////////////////////////////////////////////////////
// Button

Button::Button() : ElementGUI() {
	_isSelected = false;
}

Button::~Button() {

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
		ElementGUI_hovered = this;
	}


}

void NormalButton::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left)	{		
			ElementGUI_pressed = this;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
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
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();
}

void NormalButton::draw() {
	sf::RectangleShape rect(sf::Vector2f(_rect.size.x - 2*1.0f, _rect.size.y - 2*1.0f));
	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(tools_button_press_color);
		rect.setOutlineThickness(1.0f);
		rect.setOutlineColor(tools_button_press_border_color);
		break;
	case ButtonState::Hover:
		rect.setFillColor(tools_button_hover_color);
		rect.setOutlineThickness(1.0f);
		rect.setOutlineColor(tools_button_hover_border_color);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(tools_button_select_color);
			rect.setOutlineThickness(1.0f);
			rect.setOutlineColor(tools_button_select_border_color);
		}
		else {
			rect.setFillColor(tools_button_idle_color);
			rect.setOutlineThickness(1.0f);
			rect.setOutlineColor(tools_button_idle_border_color);
		};
		break;
	};

	rect.setPosition(sf::Vector2f(_rect.position.x + 1.0f, _rect.position.y + 1.0f));
	window->draw(rect);

	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

}

//////////////////////////////////////////////////////////////////
// NormalButtonWithText

ColoredButtonWithText::ColoredButtonWithText(std::wstring text, sf::Vector2i size, sf::Vector2i position) : Button()
 {
	_textStr = text;

	_idleColor = dark_and_red_button_normal_color;
	_hoverColor = dark_and_red_button_hover_color;
	_pressColor = dark_and_red_button_press_color;
	_selectColor = dark_and_red_button_select_color;

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
	
}

void ColoredButtonWithText::setColors(sf::Color selectColor, sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor) {
	_selectColor = selectColor;
	_idleColor = idleColor;
	_hoverColor = hoverColor;
	_pressColor = pressColor;
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
		ElementGUI_hovered = this;
	}


}

void ColoredButtonWithText::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
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
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();
}

void ColoredButtonWithText::draw() {

	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(_pressColor);
		rect.setOutlineThickness(dialog_border_width);
		rect.setOutlineColor(dialog_border_color);
		break;
	case ButtonState::Hover:
		rect.setFillColor(_hoverColor);
		rect.setOutlineThickness(dialog_border_width);
		rect.setOutlineColor(dialog_border_color);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(_selectColor);
			rect.setOutlineThickness(dialog_border_width);
			rect.setOutlineColor(dialog_border_color);
		}
		else {
			rect.setFillColor(_idleColor);
			rect.setOutlineThickness(dialog_border_width);
			rect.setOutlineColor(dialog_border_color);
		};
		break;
	};
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);

	if (!_text) {
		_text = std::make_unique<sf::Text>(basicFont, _textStr, 13);
		_text->setFillColor(dark_and_red_button_text_color);
	}

	sf::Vector2f text_pos;
	text_pos.x = _rect.position.x + _rect.size.x / 2 - _text->getGlobalBounds().size.x / 2;
	text_pos.y = _rect.position.y + _rect.size.y / 2 - basicFont.getLineSpacing(13) / 2;
	_text->setPosition(text_pos);

	window->draw(*_text);
}

//////////////////////////////////////////////////////////////////
// ButtonWithBottomText

ButtonWithBottomText::ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position) : Button()
{
	_textStr = text;
	_rectColor = rectColor;
	_textColor = textColor;
	_hoverTextColor = hoverTextColor;

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
}

void ButtonWithBottomText::unclick() {
	_state = ButtonState::Idle;
}

void ButtonWithBottomText::hover() {
	_state = ButtonState::Hover;

}

void ButtonWithBottomText::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}


void ButtonWithBottomText::cursorHover() {

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this;
	}


}

void ButtonWithBottomText::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
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
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();
}

void ButtonWithBottomText::draw() {

	sf::RectangleShape rect(sf::Vector2f(_rect.size.x - 2*tools_border_width, _rect.size.y - 2*tools_border_width));
	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(tools_button_press_color);
		rect.setOutlineThickness(tools_border_width);
		rect.setOutlineColor(tools_button_press_border_color);
		break;
	case ButtonState::Hover:
		rect.setFillColor(tools_button_hover_color);
		rect.setOutlineThickness(tools_border_width);
		rect.setOutlineColor(tools_button_hover_border_color);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(tools_button_select_color);
			rect.setOutlineThickness(tools_border_width);
			rect.setOutlineColor(tools_button_select_border_color);
		}
		else {
			rect.setFillColor(tools_button_idle_color);
			rect.setOutlineThickness(tools_border_width);
			rect.setOutlineColor(tools_button_idle_border_color);
		};
		break;
	};
	rect.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(tools_border_width, tools_border_width));
	window->draw(rect);


	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

	if (_text == nullptr) {
		_text = std::make_unique<sf::Text>(basicFont, _textStr, 13);
	}
	_text->setFillColor((_state == ButtonState::Idle) ? _textColor : _hoverTextColor);
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(48 / 2 - _text->getGlobalBounds().size.x / 2.0f, _rect.size.y - basicFont.getLineSpacing(13) - 4));

	window->draw(*_text);
}

//////////////////////////////////////////////////////////////////
// ButtonWithRightText

ButtonWithRightText::ButtonWithRightText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position) : Button()
{
	_textStr = text;

	_textColor = textColor;
	_hoverTextColor = hoverTextColor;

	_texture = texture;
	_hoverTexture = hoverTexture;

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
}

void ButtonWithRightText::unclick() {
	_state = ButtonState::Idle;

}

void ButtonWithRightText::hover() {
	_state = ButtonState::Hover;
}

void ButtonWithRightText::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}


void ButtonWithRightText::cursorHover() {

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this;
	}


}

void ButtonWithRightText::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
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
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();
}

void ButtonWithRightText::draw() {

	sf::RectangleShape rect(sf::Vector2f(_rect.size.x - 2 * tools_border_width, _rect.size.y - 2 * tools_border_width));
	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(tools_button_press_color);
		rect.setOutlineThickness(tools_border_width);
		rect.setOutlineColor(tools_button_press_border_color);
		break;
	case ButtonState::Hover:
		rect.setFillColor(tools_button_hover_color);
		rect.setOutlineThickness(tools_border_width);
		rect.setOutlineColor(tools_button_hover_border_color);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(tools_button_select_color);
			rect.setOutlineThickness(tools_border_width);
			rect.setOutlineColor(tools_button_select_border_color);
		}
		else {
			rect.setFillColor(tools_button_idle_color);
			rect.setOutlineThickness(tools_border_width);
			rect.setOutlineColor(tools_button_idle_border_color);
		};
		break;
	};
	rect.setPosition(sf::Vector2f(_rect.position.x + tools_border_width, _rect.position.y + tools_border_width));
	window->draw(rect);

	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

	if (_text == nullptr) {
		_text = std::make_unique<sf::Text>(basicFont, _textStr, 13);
	}
	_text->setFillColor((_state == ButtonState::Idle) ? _textColor : _hoverTextColor);
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(32, 24 - basicFont.getLineSpacing(13)));
	window->draw(*_text);
}

//////////////////////////////////////////////////////////////////
// Option

Option::Option(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position) : Button() {

	_textStr = text;

	_texture = texture;
	_hoverTexture = hoverTexture;
	
	sf::Text _text(basicFont, text, 13);
	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32 + _text.getGlobalBounds().size.x + 8, 32));

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
		ElementGUI_hovered = this;
	}


}

void Option::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
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
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();
}

void Option::draw() {

	// draw rectangle
	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(optionbox_press_color);
		break;
	case ButtonState::Hover:
		rect.setFillColor(optionbox_hover_color);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(optionbox_select_color);
		}
		else {
			rect.setFillColor(optionbox_idle_color);
		};
		break;
	};
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);

	// Draw sprite
	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

	// draw text
	if(_text == nullptr) {
		_text = std::make_unique<sf::Text>(basicFont, _textStr, 13);
	}
	_text->setFillColor(menu_text_color);
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(32, 24 - basicFont.getLineSpacing(13)));
	window->draw(*_text);
}

//////////////////////////////////////////////////////////////////
// ButtonWithTopTextAndList

ButtonWithTopTextAndList::ButtonWithTopTextAndList(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, sf::Vector2i position) : Button() {

	_textStr = text;

	_textColor = textColor;
	_hoverTextColor = hoverTextColor;
	
	_texture = getTexture(L"tex\\tools\\bottom_arrow.png");
	_hoverTexture = getTexture(L"tex\\tools\\bottom_arrow_hover.png");
	
	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(48, 32));

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
		_list_rect.setSize(sf::Vector2f(0, o->_rect.size.y + menuoptions_border_width));
		return;
	}

	int wdt = (_options.back()->_rect.size.x > o->_rect.size.x) ? _options.back()->_rect.size.x : o->_rect.size.x;

	_list_rect.setSize(sf::Vector2f(wdt + 2 * menuoptions_border_width, _list_rect.getSize().y + o->_rect.size.y));

	_options.push_back(o);
	for (auto& o : _options) {
		o->_rect.size = sf::Vector2i(wdt, 32);
	}
}

void ButtonWithTopTextAndList::setPosition(sf::Vector2i position) {
	
	_rect.position = position;

	sf::Vector2i pos = _rect.position + sf::Vector2i(0, _rect.size.y);

	this->_list_rect.setPosition(sf::Vector2f(pos.x - menuoptions_border_width, pos.y));
	for (auto& o : _options) {
		o->setPosition(pos);
		pos.y += 32;
	}
}

void ButtonWithTopTextAndList::unclick() {
	_state = ButtonState::Idle;
	
}

void ButtonWithTopTextAndList::hover() {
	_state = ButtonState::Hover;
}

void ButtonWithTopTextAndList::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}


void ButtonWithTopTextAndList::cursorHover() {

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	if (_isOpen) {
		for (auto& option : _options)
			option->cursorHover();
	}


}

void ButtonWithTopTextAndList::handleEvent(const sf::Event& event) {

	if (_rect.contains(cursor->_worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
				click();
				_isOpen = !_isOpen;
			}
		}
	}

	bool clicked_in_menu = false;

	if (_isOpen) {
		for (auto& option : _options) {
			option->handleEvent(event);
			if (ElementGUI_pressed == option.get())
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
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();


	for (auto& option : _options)
		option->update();
}

void ButtonWithTopTextAndList::draw() {

	// draw rect
	sf::RectangleShape rect(sf::Vector2f(_rect.size - sf::Vector2i(2*tools_border_width, 2*tools_border_width)));
	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(tools_button_press_color);
		rect.setOutlineThickness(tools_border_width);
		rect.setOutlineColor(tools_button_press_border_color);
		break;
	case ButtonState::Hover:
		rect.setFillColor(tools_button_hover_color);
		rect.setOutlineThickness(tools_border_width);
		rect.setOutlineColor(tools_button_hover_border_color);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(tools_button_select_color);
			rect.setOutlineThickness(tools_border_width);
			rect.setOutlineColor(tools_button_select_border_color);
		}
		else {
			rect.setFillColor(tools_button_idle_color);
			rect.setOutlineThickness(tools_border_width);
			rect.setOutlineColor(tools_button_idle_border_color);
		};
		break;
	};

	if (_isOpen) {
		rect.setFillColor(tools_button_press_color);
		rect.setOutlineThickness(tools_border_width);
		rect.setOutlineColor(tools_button_press_border_color);
	}
	
	rect.setPosition(sf::Vector2f(_rect.position.x + tools_border_width, _rect.position.y + tools_border_width));
	window->draw(rect);

	// draw sprite
	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position.x, _rect.position.y + 16));
	window->draw(sprite);

	// draw text
	if (_text == nullptr) {
		_text = std::make_unique<sf::Text>(basicFont, _textStr, 13);
	}
	_text->setFillColor((_state == ButtonState::Idle) ? _textColor : _hoverTextColor);
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(48 / 2 - _text->getGlobalBounds().size.x / 2.0f, 0));
	window->draw(*_text);

	if (_isOpen) {

		window->draw(_list_rect);
		for (auto& o : _options) {
			o->draw();
		}

	}
}