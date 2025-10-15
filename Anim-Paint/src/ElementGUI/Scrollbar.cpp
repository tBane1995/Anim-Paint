#include "ElementGUI/Scrollbar.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include <iostream>

Scrollbar::Scrollbar(float x, float y, float width, float height, int min_value, int max_value, int slider_size, int value) {
	
	

	_rect = sf::RectangleShape(sf::Vector2f(width, height));
	_rect.setFillColor(sf::Color(79, 79, 79));

	_slider = sf::RectangleShape(sf::Vector2f(width, width));
	_slider.setFillColor(sf::Color(47,47,47));

	_min_value = min_value;
	_max_value = max_value;
	_slider_size = slider_size;
	_value = value;

	updateSliderSize();
	setPosition(x, y);

	_state = ScrollbarState::Idle;
	_func = {};
	_dragOffset = sf::Vector2f(0, 0);

	_scrollArea = nullptr;
}


Scrollbar::~Scrollbar() {

}

void Scrollbar::setMin(int min_value) {

	_min_value = min_value;
	updateSliderSize();
}

void Scrollbar::setMax(int max_value) {
	_max_value = max_value;
	if(_max_value < _min_value)
		_max_value = _min_value;
	updateSliderSize();
}

void Scrollbar::setValue(int value) {
	_value = std::clamp(value, _min_value, _max_value);

	updateSliderPosition();
}

void Scrollbar::setScrollArea(sf::RectangleShape* rect, float scrollStep) {
	_scrollArea = rect;
	_scrollStep = scrollStep;
}

void Scrollbar::updateSliderSize() {
	sf::Vector2f slider = _rect.getSize();
	int range = _max_value - _min_value;

	if (range <= 0) {                               // treść mieści się w oknie
		_slider.setSize({ slider.x, slider.y });        // kciuk = pełen tor
		return;
	}

	float sizeY = slider.y * ((float)_slider_size / ((float)range + (float)_slider_size));

	_slider.setSize({ slider.x, sizeY });
}

void Scrollbar::updateSliderPosition() {
	int range = _max_value - _min_value;

	sf::Vector2f pos = _rect.getPosition();

	if (range <= 0) {                // nic do przewijania
		_slider.setPosition(pos);
		return;
	}

	pos.y += ((float)(_value - _min_value) / (float)range) * (_rect.getSize().y - _slider.getSize().y);

	_slider.setPosition(pos);
}

void Scrollbar::setPosition(float x, float y) {
	_rect.setPosition(sf::Vector2f(x, y));
	updateSliderPosition();
}

int Scrollbar::getValue() {
	return _value;
}

void Scrollbar::cursorHover() {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void Scrollbar::handleEvent(const sf::Event& event) {

	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (ElementGUI_pressed == this) {
			_state = ScrollbarState::Idle;
		}
	}
	else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_slider.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_pressed = this;
			if (_slider.getSize().y < _rect.getSize().y) {
				_state = ScrollbarState::Dragging;
				_dragOffset = worldMousePosition - _slider.getPosition();
			}
		}
		
	}
	else if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>(); mws) {
		if (_rect.getGlobalBounds().contains(worldMousePosition) || _scrollArea->getGlobalBounds().contains(worldMousePosition)) {
			_state = ScrollbarState::Scrolled;
			_deltaScroll = -mws->delta;
		}
	}
	

}

void Scrollbar::update() {
	if (_state == ScrollbarState::Dragging) {
		sf::Vector2f position = sf::Vector2f(_slider.getPosition().x, worldMousePosition.y - _dragOffset.y);
		position.y = std::clamp(position.y, _rect.getPosition().y, _rect.getPosition().y + _rect.getSize().y - _slider.getSize().y);
		int val = (int)(((position.y - _rect.getPosition().y) / (_rect.getSize().y - _slider.getSize().y)) * (_max_value - _min_value)) + _min_value;
		if (val != _value) {
			_value = val;
			updateSliderPosition();
			_func();
		}
	}

	if (_state == ScrollbarState::Scrolled) {
		setValue(_value + float(_deltaScroll)*_scrollStep);
		_func();
		_state = ScrollbarState::Idle;
	}
}

void Scrollbar::draw() {
	window->draw(_rect);
	window->draw(_slider);
}