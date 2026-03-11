#include "Components/MainMenu/OptionBox.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

OptionBox::OptionBox(std::wstring text) : Element() {
	_text = std::make_unique<sf::Text>(basicFont, text, menu_font_size);
	_text->setFillColor(menu_text_color);

	sf::Vector2f rectSize;
	rectSize.x = _text->getGlobalBounds().size.x + (float)(2 * menu_horizontal_margin);
	rectSize.y = (float)menu_height;

	_rect = sf::RectangleShape(rectSize);
	_rect.setFillColor(optionbox_idle_color);

	_state = ButtonState::Idle;
	_onclick_func = { };
}

OptionBox::~OptionBox() {

}

void OptionBox::setPosition(sf::Vector2i position) {
	_rect.setPosition(sf::Vector2f(position));
	
	sf::Vector2f textPos;
	textPos.x = (float)(position.x + menu_horizontal_margin);
	textPos.y = (float)(position.y) + ((float)menu_height - basicFont.getLineSpacing(menu_font_size)) / 2.0f;
	_text->setPosition(textPos);
}

void OptionBox::unclick() {
	_state = ButtonState::Idle;
	_rect.setFillColor(optionbox_idle_color);
}

void OptionBox::hover() {
	_state = ButtonState::Hover;
	_rect.setFillColor(optionbox_hover_color);

}

void OptionBox::click() {
	_state = ButtonState::Pressed;
	_rect.setFillColor(optionbox_press_color);
	_clickTime = currentTime;
}


void OptionBox::cursorHover() {
	if (_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_position))) {
		Element_hovered = this->shared_from_this();
	}
}

void OptionBox::handleEvent(const sf::Event& event)
{
	if (!_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_position)))
		return;

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left)
	{
		Element_pressed = this->shared_from_this();
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left)
	{
		if (Element_pressed.get() == this)
			click();
	}
}

void OptionBox::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}

			if(Element_pressed.get() == this)
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

void OptionBox::draw() {
	window->draw(_rect);
	window->draw(*_text);
}