#include "ElementGUI/BigSlider.hpp"
#include "ElementGUI/ElementGUI.hpp"
#include "Textures.hpp"
#include "SFML/Graphics.hpp"
#include "Cursor.hpp"
#include "Window.hpp"
#include "Theme.hpp"

BigSlider::BigSlider(std::wstring name, int min_value, int max_value) : ElementGUI() {
	_name = name;

	_state = BigSliderState::Idle;
	_editState = BigSliderEditState::None;

	_nameText = std::make_unique<sf::Text>(basicFont, _name, slider_font_size);
	_nameText->setFillColor(slider_text_color);

	_valueText = std::make_unique<sf::Text>(basicFont, std::to_wstring(_current_value), slider_font_size);
	_valueText->setFillColor(slider_text_color);

	_min_value = min_value;
	_max_value = max_value;
	setValue(min_value);

	_barRect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(160, 24));
	_thumbRect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(24, 24));
	
}

BigSlider::~BigSlider() {

}

void BigSlider::setValue(int value) {
	_current_value = value;

	if (_current_value < _min_value)
		_current_value = _min_value;
	else if (_current_value > _max_value)
		_current_value = _max_value;

	_valueText->setString(std::to_wstring(_current_value));
	_thumbRect.size.x = getThumbWidth();
}

int BigSlider::getValue() {
	return _current_value;
}

sf::Vector2i BigSlider::getSize() {
	return _barRect.size;
}

int BigSlider::getThumbWidth() {
	float sliderWidth = float(_current_value - _min_value) / float(_max_value - _min_value) * float(_barRect.size.x);
	return (int)(sliderWidth);
}

void BigSlider::setPosition(sf::Vector2i position) {
	_barRect.position = position;
	_thumbRect.position = position;
	
	int text_margin = 6;
	_nameText->setPosition(sf::Vector2f(position.x + text_margin, position.y + _barRect.size.y / 2 - basicFont.getLineSpacing(slider_font_size) / 2));
	_valueText->setPosition(sf::Vector2f(position.x + _barRect.size.x - text_margin - _valueText->getGlobalBounds().size.x, position.y + _barRect.size.y / 2 - basicFont.getLineSpacing(slider_font_size) / 2));
}

void BigSlider::cursorHover() {
	if(_barRect.contains(cursor->_worldMousePosition))
		ElementGUI_hovered = this->shared_from_this();
}

void BigSlider::handleEvent(const sf::Event& event) {

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_barRect.contains(cursor->_worldMousePosition)) {
			ElementGUI_pressed = this->shared_from_this();
			_editState = BigSliderEditState::Changed;

		}
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (ElementGUI_pressed.get() == this) {
			ElementGUI_pressed = nullptr;
			_state = BigSliderState::Idle;
			_editState = BigSliderEditState::None;
		}
	}
}

void BigSlider::update() {
	if (_editState == BigSliderEditState::Changed) {
		_thumbRect.size.x = cursor->_worldMousePosition.x - _barRect.position.x;
		_thumbRect.size.x = std::max(0, std::min(_thumbRect.size.x, _barRect.size.x));
		int newValue = (int)((float)_thumbRect.size.x / (float)_barRect.size.x * float(_max_value - _min_value) + float(_min_value));
		setValue(newValue);
		_state = BigSliderState::Pressed;
		return;
	}

	if (ElementGUI_pressed.get() == this) {
		_state = BigSliderState::Pressed;
	}else if(ElementGUI_hovered .get() == this) {
		_state = BigSliderState::Hovered;
	}
	else {
		_state = BigSliderState::Idle;
	}
}


void BigSlider::draw() {
	// Bar
	sf::RectangleShape barRect(sf::Vector2f(_barRect.size));
	barRect.setPosition(sf::Vector2f(_barRect.position));
	switch (_state) {
		case BigSliderState::Idle:
			barRect.setFillColor(slider_bar_idle_color);
			break;
		case BigSliderState::Hovered:
			barRect.setFillColor(slider_bar_hover_color);
			break;
		case BigSliderState::Pressed:
			barRect.setFillColor(slider_bar_press_color);
			break;
	}
	window->draw(barRect);

	// Thumb
	sf::RectangleShape thumbRect(sf::Vector2f(_thumbRect.size));
	thumbRect.setPosition(sf::Vector2f(_thumbRect.position));

	if (_editState == BigSliderEditState::Changed) {
		thumbRect.setFillColor(slider_thumb_press_color);
	}
	else {
		switch (_state) {
		case BigSliderState::Idle:
			thumbRect.setFillColor(slider_thumb_idle_color);
			break;
		case BigSliderState::Hovered:
			thumbRect.setFillColor(slider_thumb_hover_color);
			break;
		case BigSliderState::Pressed:
			thumbRect.setFillColor(slider_thumb_press_color);
			break;
		}
	}
		
	
	window->draw(thumbRect);

	// Texts
	window->draw(*_nameText);
	window->draw(*_valueText);
}