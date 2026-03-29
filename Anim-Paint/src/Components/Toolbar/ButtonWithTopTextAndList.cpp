#include "Components/Toolbar/ButtonWithTopTextAndList.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

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
	_list_rect.setFillColor(optionbox_border_color);
	_options.clear();
	_isOpen = false;

	_state = ButtonState::Idle;

	_hover_func = { };
	_onclick_func = { };
	_clickTime = currentTime;

	
}

ButtonWithTopTextAndList::~ButtonWithTopTextAndList() {

}

void ButtonWithTopTextAndList::addOption(std::shared_ptr<Option> option) {

	if (_options.size() == 0) {
		_options.push_back(option);
		sf::Vector2f rectSize;
		rectSize.x = 0;
		rectSize.y = float(option->_rect.size.y + optionbox_border_width);
		_list_rect.setSize(rectSize);
		return;
	}

	int wdt = (_options.back()->_rect.size.x > option->_rect.size.x) ? _options.back()->_rect.size.x : option->_rect.size.x;
	sf::Vector2f rectSize;
	rectSize.x = float(wdt + 2 * optionbox_border_width);
	rectSize.y = float(_list_rect.getSize().y + option->_rect.size.y);
	_list_rect.setSize(rectSize);

	_options.push_back(option);
	for (auto& o : _options) {
		o->setSize(sf::Vector2i(wdt, 32));
	}

	
}

void ButtonWithTopTextAndList::setPosition(sf::Vector2i position) {

	_rect.position = position;

	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(48 / 2 - _text->getGlobalBounds().size.x / 2.0f, 0));

	sf::Vector2i ipos = _rect.position + sf::Vector2i(0, _rect.size.y);

	int maxTextWidth = 0;
	for (auto& o : _options) {
		if(o->getTextWidth() > maxTextWidth)
			maxTextWidth = o->getTextWidth();
	}

	int maxShortcutTextWidth = 0;
	for (auto& o : _options) {
		if (o->getShortcutTextWidth() > maxShortcutTextWidth)
			maxShortcutTextWidth = o->getShortcutTextWidth();
	}

	sf::Vector2f rectPos;
	rectPos.x = float(ipos.x - optionbox_border_width);
	rectPos.y = float(ipos.y);

	_list_rect.setPosition(rectPos);
	for (auto& o : _options) {
		o->setSize(sf::Vector2i(32 + maxTextWidth + maxShortcutTextWidth + 8, 32));
		o->setPosition(ipos, maxTextWidth);
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
	_isOpen = true;
	_text->setFillColor(_hoverTextColor);
}


void ButtonWithTopTextAndList::cursorHover() {

	Button::cursorHover();

	if (_isOpen) {
		for (auto& option : _options)
			option->cursorHover();
	}


}

void ButtonWithTopTextAndList::handleEvent(const sf::Event& event) {

	if (Element_hovered.get() == this) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			Element_pressed = this->shared_from_this();
		}
		
	}

	if (Element_pressed.get() == this) {
		click();
	}

	bool clicked_in_menu = false;
	if(Element_pressed.get() == this) {
		clicked_in_menu = true;
	}

	if (_isOpen) {
		for (auto& option : _options) {
			option->handleEvent(event);

			if (option->_rect.contains(cursor->_position)) {
				clicked_in_menu = true;
				break;
			}
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
	sprite.setPosition(rectPosition + sf::Vector2f(0, 16.0f));
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