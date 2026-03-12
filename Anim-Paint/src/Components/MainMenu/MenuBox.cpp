#include "Components/MainMenu/MenuBox.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

MenuBox::MenuBox(std::wstring text) : Button() {

	_text = std::make_unique<sf::Text>(basicFont, text, menu_font_size);
	_text->setFillColor(menu_text_color);

	sf::Vector2i rectSize;
	_rect.size.x = (int)(_text->getGlobalBounds().size.x + (float)(2 * menu_horizontal_margin));
	_rect.size.y = menu_height;

	_rectIdleColor = menubox_idle_color;
	_rectHoverColor = menubox_hover_color;
	_rectPressColor = menubox_press_color;
	_rectSelectColor = menubox_open_color;

	_state = ButtonState::Idle;
	_isSelected = false;
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
	_rect.position = position;

	sf::Vector2f textPos;
	textPos.x = (float)(position.x + menu_horizontal_margin);
	textPos.y = (float)(position.y) + ((float)(menu_height) - basicFont.getLineSpacing(menu_font_size)) / 2.0f;
	_text->setPosition(textPos);

	for (int i = 0; i < _options.size(); i++) {
		sf::Vector2i optionPos;
		optionPos.x = (int)(_rect.position.x);
		optionPos.y = (int)(_rect.position.y + _rect.size.y) + i * menu_height;
		_options[i]->setPosition(optionPos);
	}
}




void MenuBox::cursorHover() {
	Button::cursorHover();

	if (Element_pressed.get() == this) {
		_isSelected = true;
	}

	if (_isSelected) {
		for (auto& option : _options)
			option->cursorHover();
	}
}

void MenuBox::handleEvent(const sf::Event& event) {
	Button::handleEvent(event);

	if (_isSelected) {
		for (auto& option : _options)
			option->handleEvent(event);
	}
}

void MenuBox::update() {
	Button::update();

	for (auto& option : _options)
		option->update();
}

void MenuBox::draw() {
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

	if (_isSelected) {
		for (auto& option : _options)
			option->draw();
	}
}
