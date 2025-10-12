#include "ElementGUI/Button.hpp"
#include "SFML/Graphics.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Mouse.hpp"
#include "Window.hpp"

//////////////////////////////////////////////////////////////////
// Button

Button::Button() : ElementGUI() {

}

Button::~Button() {

}

sf::Vector2f Button::getSize() { 
	return sf::Vector2f(0, 0); 
}

void Button::setPosition(sf::Vector2f position) {

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

NormalButton::NormalButton(Texture* texture, Texture* hoverTexture, sf::Vector2f position)
: _sprite(*texture->_texture) {

	_texture = texture;
	_hoverTexture = hoverTexture;
	_rect = sf::RectangleShape(sf::Vector2f(_texture->_texture->getSize()) - sf::Vector2f(2 * tools_border_width, 2 * tools_border_width));
	_rect.setOutlineThickness(1.0f);
	_rect.setFillColor(sf::Color::Transparent);
	_rect.setOutlineColor(sf::Color::Transparent);

	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

NormalButton::~NormalButton() {}

sf::Vector2f NormalButton::getSize() {
	return _rect.getGlobalBounds().size;
}

void NormalButton::setPosition(sf::Vector2f position) {
	_position = position;
	_sprite.setPosition(_position);
	_rect.setPosition(_position + sf::Vector2f(tools_border_width, tools_border_width));
}

void NormalButton::unclick() {
	_state = ButtonState::Idle;
	_sprite.setTexture(*_texture->_texture);
	_rect.setFillColor(tools_button_idle_color);
	_rect.setOutlineColor(tools_button_idle_border_color);
}

void NormalButton::hover() {
	_state = ButtonState::Hover;
	_sprite.setTexture(*_hoverTexture->_texture);
	_rect.setFillColor(tools_button_hover_color);
	_rect.setOutlineColor(tools_button_hover_border_color);

}

void NormalButton::click() {
	_state = ButtonState::Pressed;
	_sprite.setTexture(*_hoverTexture->_texture);
	_clickTime = currentTime;
	_rect.setFillColor(tools_button_press_color);
	_rect.setOutlineColor(tools_button_press_border_color);
}


void NormalButton::cursorHover() {

	if (_sprite.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}


}

void NormalButton::handleEvent(const sf::Event& event) {
	if (_sprite.getGlobalBounds().contains(worldMousePosition)) {
		
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
	window->draw(_rect);
	window->draw(_sprite);

}

//////////////////////////////////////////////////////////////////
// NormalButtonWithText

NormalButtonWithText::NormalButtonWithText(std::wstring text, sf::Vector2f size, sf::Vector2f position):
_text(basicFont, text, 13) {

	_idleColor = button_normal_color;
	_hoverColor = button_hover_color;
	_pressColor = button_press_color;

	_rect = sf::RectangleShape(size);
	_rect.setFillColor(_idleColor);
	_rect.setOutlineThickness(dialog_border_width);
	_rect.setOutlineColor(sf::Color(dialog_border_color));

	_text.setFillColor(button_text_color);

	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

NormalButtonWithText::~NormalButtonWithText() {}

sf::Vector2f NormalButtonWithText::getSize() {
	return _rect.getSize();
}

void NormalButtonWithText::setPosition(sf::Vector2f position) {
	_position = position;
	_rect.setPosition(_position);
	sf::Vector2f text_pos;
	text_pos.x = _rect.getPosition().x + _rect.getSize().x / 2 - _text.getGlobalBounds().size.x / 2;
	text_pos.y = _rect.getPosition().y + _rect.getSize().y / 2 - basicFont.getLineSpacing(13) / 2;
	_text.setPosition(text_pos);
}

void NormalButtonWithText::setColors(sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor) {
	_idleColor = idleColor;
	_hoverColor = hoverColor;
	_pressColor = pressColor;
}

void NormalButtonWithText::unclick() {
	_state = ButtonState::Idle;
	_rect.setFillColor(_idleColor);
}

void NormalButtonWithText::hover() {
	_state = ButtonState::Hover;
	_rect.setFillColor(_hoverColor);
}

void NormalButtonWithText::click() {
	_state = ButtonState::Pressed;
	_rect.setFillColor(_pressColor);
	_clickTime = currentTime;
}


void NormalButtonWithText::cursorHover() {

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}


}

void NormalButtonWithText::handleEvent(const sf::Event& event) {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
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

void NormalButtonWithText::update() {

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

void NormalButtonWithText::draw() {
	window->draw(_rect);
	window->draw(_text);
}

//////////////////////////////////////////////////////////////////
// ButtonWithBottomText

ButtonWithBottomText::ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, Texture* texture, Texture* hoverTexture, sf::Vector2f position):
	_text(basicFont, text, 13),
	_sprite(*texture->_texture)
{

	_textColor = textColor;
	_hoverTextColor = hoverTextColor;

	_text.setFillColor(textColor);

	_texture = texture;
	_hoverTexture = hoverTexture;

	_rect = sf::RectangleShape(sf::Vector2f(48 - 2 * tools_border_width, 64 - 2 * tools_border_width));
	_rect.setFillColor(rectColor);
	_rect.setOutlineThickness(tools_border_width);
	_rect.setOutlineColor(sf::Color::Transparent);
	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

ButtonWithBottomText::~ButtonWithBottomText() {}

sf::Vector2f ButtonWithBottomText::getSize() {
	return _rect.getGlobalBounds().size;
}

void ButtonWithBottomText::setPosition(sf::Vector2f position) {
	_position = position;
	_rect.setPosition(_position + sf::Vector2f(tools_border_width, tools_border_width));
	_sprite.setPosition(_position);
	_text.setPosition(_position + sf::Vector2f(48 / 2 - _text.getGlobalBounds().size.x / 2.0f, _rect.getSize().y - basicFont.getLineSpacing(13) - 4));
}

void ButtonWithBottomText::unclick() {
	_state = ButtonState::Idle;
	_sprite.setTexture(*_texture->_texture);
	_text.setFillColor(_textColor);
	_rect.setFillColor(tools_button_idle_color);
	_rect.setOutlineColor(tools_button_idle_border_color);
}

void ButtonWithBottomText::hover() {
	_state = ButtonState::Hover;
	_sprite.setTexture(*_hoverTexture->_texture);
	_text.setFillColor(_hoverTextColor);
	_rect.setFillColor(tools_button_hover_color);
	_rect.setOutlineColor(tools_button_hover_border_color);

}

void ButtonWithBottomText::click() {
	_state = ButtonState::Pressed;
	_sprite.setTexture(*_hoverTexture->_texture);
	_text.setFillColor(_hoverTextColor);
	_clickTime = currentTime;
	_rect.setFillColor(tools_button_press_color);
	_rect.setOutlineColor(tools_button_press_border_color);
}


void ButtonWithBottomText::cursorHover() {

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}


}

void ButtonWithBottomText::handleEvent(const sf::Event& event) {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
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
	window->draw(_rect);
	window->draw(_sprite);
	window->draw(_text);
}

//////////////////////////////////////////////////////////////////
// ButtonWithRightText

ButtonWithRightText::ButtonWithRightText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, Texture* texture, Texture* hoverTexture, sf::Vector2f position) :
	_text(basicFont, text, 13),
	_sprite(*texture->_texture)
{
	_textColor = textColor;
	_hoverTextColor = hoverTextColor;

	_text.setFillColor(textColor);

	_texture = texture;
	_hoverTexture = hoverTexture;

	_rect = sf::RectangleShape(sf::Vector2f(64 - 2 * tools_border_width, 32 - 2 * tools_border_width));
	_rect.setFillColor(rectColor);
	_rect.setOutlineThickness(tools_border_width);
	_rect.setOutlineColor(sf::Color::Transparent);
	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

ButtonWithRightText::~ButtonWithRightText() {}

sf::Vector2f ButtonWithRightText::getSize() {
	return _rect.getGlobalBounds().size;
}

void ButtonWithRightText::setPosition(sf::Vector2f position) {
	_position = position;
	_rect.setPosition(_position);
	_sprite.setPosition(_position);
	_text.setPosition(_position + sf::Vector2f(32, 24 - basicFont.getLineSpacing(13)));
}

void ButtonWithRightText::unclick() {
	_state = ButtonState::Idle;
	_sprite.setTexture(*_texture->_texture);
	_text.setFillColor(_textColor);

	_rect.setFillColor(tools_button_idle_color);
	_rect.setOutlineColor(tools_button_idle_border_color);
}

void ButtonWithRightText::hover() {
	_state = ButtonState::Hover;
	_sprite.setTexture(*_hoverTexture->_texture);
	_text.setFillColor(_hoverTextColor);

	_rect.setFillColor(tools_button_hover_color);
	_rect.setOutlineColor(tools_button_hover_border_color);

}

void ButtonWithRightText::click() {
	_state = ButtonState::Pressed;
	_sprite.setTexture(*_hoverTexture->_texture);
	_text.setFillColor(_hoverTextColor);
	_clickTime = currentTime;

	_rect.setFillColor(tools_button_press_color);
	_rect.setOutlineColor(tools_button_press_border_color);
}


void ButtonWithRightText::cursorHover() {

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}


}

void ButtonWithRightText::handleEvent(const sf::Event& event) {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
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
	window->draw(_rect);
	window->draw(_sprite);
	window->draw(_text);
}

//////////////////////////////////////////////////////////////////
// Option

Option::Option(std::wstring text, Texture* texture, Texture* hoverTexture, sf::Vector2f position):
	_text(basicFont, text, 13),
	_sprite(*texture->_texture) {

	_text.setFillColor(menu_text_color);

	_texture = texture;
	_hoverTexture = hoverTexture;

	_rect = sf::RectangleShape(sf::Vector2f(32 + _text.getGlobalBounds().size.x + 8, 32));
	_rect.setFillColor(optionbox_idle_color);
	setPosition(position);

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

}

Option::~Option() {}

sf::Vector2f Option::getSize() {
	return _rect.getGlobalBounds().size;
}

void Option::setPosition(sf::Vector2f position) {
	_position = position;
	_rect.setPosition(_position);
	_sprite.setPosition(_position);
	_text.setPosition(_position + sf::Vector2f(32, 24 - basicFont.getLineSpacing(13)));
}

void Option::unclick() {
	_state = ButtonState::Idle;
	_sprite.setTexture(*_texture->_texture);
	_rect.setFillColor(optionbox_idle_color);
	_text.setFillColor(menu_text_color);
}

void Option::hover() {
	_state = ButtonState::Hover;
	_sprite.setTexture(*_hoverTexture->_texture);
	_rect.setFillColor(optionbox_hover_color);
	_text.setFillColor(menu_text_color);

}

void Option::click() {
	_state = ButtonState::Pressed;
	_sprite.setTexture(*_hoverTexture->_texture);
	_rect.setFillColor(optionbox_press_color);
	_text.setFillColor(menu_text_color);
	_clickTime = currentTime;
}


void Option::cursorHover() {

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}


}

void Option::handleEvent(const sf::Event& event) {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
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
	window->draw(_rect);
	window->draw(_sprite);
	window->draw(_text);
}

//////////////////////////////////////////////////////////////////
// ButtonWithTopTextAndList

ButtonWithTopTextAndList::ButtonWithTopTextAndList(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, sf::Vector2f position) :
	_text(basicFont, text, 13),
	_sprite(*getTexture(L"tex\\tools\\bottom_arrow.png")->_texture) {

	_textColor = textColor;
	_hoverTextColor = hoverTextColor;
	
	_text.setFillColor(textColor);

	_texture = getTexture(L"tex\\tools\\bottom_arrow.png");
	_hoverTexture = getTexture(L"tex\\tools\\bottom_arrow_hover.png");
	

	_rect = sf::RectangleShape(sf::Vector2f(48 - 2 * tools_border_width, 32 - 2 * tools_border_width));
	_rect.setFillColor(rectColor);
	_rect.setOutlineThickness(tools_border_width);
	_rect.setOutlineColor(sf::Color::Transparent);
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

ButtonWithTopTextAndList::~ButtonWithTopTextAndList() {}

sf::Vector2f ButtonWithTopTextAndList::getSize() {
	return _rect.getGlobalBounds().size;
}

void ButtonWithTopTextAndList::addOption(std::wstring text) {
	Option* o = new Option(text, getTexture(L"tex\\tools\\btn_none.png"), getTexture(L"tex\\tools\\btn_none_hover.png"));

	if (_options.size() == 0) {
		_options.push_back(o);
		_list_rect.setSize(sf::Vector2f(0, o->_rect.getSize().y + menuoptions_border_width));
		return;
	}

	int wdt = (_options.back()->_rect.getSize().x > o->_rect.getSize().x) ? _options.back()->_rect.getSize().x : o->_rect.getSize().x;

	_list_rect.setSize(sf::Vector2f(wdt + 2 * menuoptions_border_width, _list_rect.getSize().y + o->_rect.getSize().y));

	_options.push_back(o);
	for (auto& o : _options) {
		o->_rect.setSize(sf::Vector2f(wdt, 32));
	}
}

void ButtonWithTopTextAndList::setPosition(sf::Vector2f position) {
	_position = position;
	_rect.setPosition(_position + sf::Vector2f(tools_border_width, tools_border_width));
	_sprite.setPosition(_position + sf::Vector2f(0, 16));
	_text.setPosition(_position + sf::Vector2f(48 / 2 - _text.getGlobalBounds().size.x / 2.0f, 0));


	sf::Vector2f pos = _rect.getPosition() + sf::Vector2f(0, _rect.getSize().y);

	this->_list_rect.setPosition(pos - sf::Vector2f(menuoptions_border_width, 0));
	for (auto& o : _options) {
		o->setPosition(pos);
		pos.y += 32;
	}
}

void ButtonWithTopTextAndList::unclick() {
	_state = ButtonState::Idle;
	_sprite.setTexture(*_texture->_texture);
	_text.setFillColor(_textColor);
	_rect.setFillColor(tools_button_idle_color);
	_rect.setOutlineColor(tools_button_idle_border_color);
}

void ButtonWithTopTextAndList::hover() {
	_state = ButtonState::Hover;
	_sprite.setTexture(*_hoverTexture->_texture);
	_text.setFillColor(_hoverTextColor);
	_rect.setFillColor(tools_button_hover_color);
	_rect.setOutlineColor(tools_button_hover_border_color);
}

void ButtonWithTopTextAndList::click() {
	_state = ButtonState::Pressed;
	_sprite.setTexture(*_hoverTexture->_texture);
	_text.setFillColor(_hoverTextColor);
	_clickTime = currentTime;
	_rect.setFillColor(tools_button_press_color);
	_rect.setOutlineColor(tools_button_press_border_color);
}


void ButtonWithTopTextAndList::cursorHover() {

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	if (_isOpen) {
		for (auto& option : _options)
			option->cursorHover();
	}


}

void ButtonWithTopTextAndList::handleEvent(const sf::Event& event) {

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {

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
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();

	if (_isOpen) {
		_rect.setFillColor(tools_button_press_color);
		_rect.setOutlineColor(tools_button_press_border_color);
	}

	for (auto& option : _options)
		option->update();
}

void ButtonWithTopTextAndList::draw() {
	window->draw(_rect);
	window->draw(_sprite);
	window->draw(_text);

	if (_isOpen) {

		window->draw(_list_rect);
		for (auto& o : _options) {
			o->draw();
		}

	}
}