#include "Components/MainMenu/MenuBox.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

MenuBox::MenuBox(std::wstring text) : Element() {

	_text = std::make_unique<sf::Text>(basicFont, text, menu_font_size);
	_text->setFillColor(menu_text_color);

	sf::Vector2f rectSize;
	rectSize.x = _text->getGlobalBounds().size.x + (float)(2 * menu_horizontal_margin);
	rectSize.y = (float)(menu_height);
	_rect = sf::RectangleShape(rectSize);

	_state = ButtonState::Idle;
	_isOpen = false;
	_options.clear();
}

MenuBox::~MenuBox() {

}

void MenuBox::addOption(std::shared_ptr<OptionBox> option) {
	_options.push_back(option);

	int max_wdt = 0;
	for (auto& o : _options) {
		if ((int)(o->getSize().x) > max_wdt)
			max_wdt = (int)(o->getSize().x);
	}

	for (auto& o : _options) {
		sf::Vector2i size;
		size.x = max_wdt;
		size.y = menu_height;
		o->setSize(size);
	}


}

void MenuBox::setPosition(sf::Vector2i position) {
	_rect.setPosition(sf::Vector2f(position));

	sf::Vector2f textPos;
	textPos.x = (float)(position.x + menu_horizontal_margin);
	textPos.y = (float)(position.y) + ((float)(menu_height) - basicFont.getLineSpacing(menu_font_size)) / 2.0f;
	_text->setPosition(textPos);

	for (int i = 0; i < _options.size(); i++) {
		sf::Vector2i optionPos;
		optionPos.x = (int)(_rect.getPosition().x);
		optionPos.y = (int)(_rect.getPosition().y + _rect.getSize().y) + i * menu_height;
		_options[i]->setPosition(optionPos);
	}
}

void MenuBox::unclick() {
	_state = ButtonState::Idle;
	if (_isOpen) {
		_rect.setFillColor(menubox_open_color);
	}
	else {
		_rect.setFillColor(menubox_idle_color);
	}
}

void MenuBox::hover() {
	_state = ButtonState::Hover;
	_rect.setFillColor(menubox_hover_color);

}

void MenuBox::click() {
	_state = ButtonState::Pressed;
	_rect.setFillColor(menubox_press_color);
	_clickTime = currentTime;
}


void MenuBox::cursorHover() {
	if (_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_position))) {
		Element_hovered = this->shared_from_this();
	}

	if (_isOpen) {
		for (auto& option : _options)
			option->cursorHover();
	}
}

void MenuBox::handleEvent(const sf::Event& event) {
	if (_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_position))) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			Element_pressed = this->shared_from_this();
		}
		else if (const auto* mbp = event.getIf < sf::Event::MouseButtonReleased > (); mbp && mbp->button == sf::Mouse::Button::Left) {
			if (Element_pressed.get() == this) {
				click();
			}
		}

	}

	if (_isOpen) {
		for (auto& option : _options)
			option->handleEvent(event);
	}
}

void MenuBox::update() {

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

void MenuBox::draw() {
	window->draw(_rect);
	window->draw(*_text);

	if (_isOpen) {
		for (auto& option : _options)
			option->draw();
	}
}
