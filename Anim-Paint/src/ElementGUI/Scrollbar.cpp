#include "ElementGUI/Scrollbar.hpp"
#include "Window.hpp"
#include "Mouse.hpp"
#include <iostream>

Scrollbar::Scrollbar(float x, float y, float width, float height, int min_value, int max_value, int slider_size, int value) {
	
	_min_value = min_value;
	_max_value = max_value;
	_slider_size = slider_size;
	_value = value;

	_rect = sf::RectangleShape(sf::Vector2f(width, height));
	_rect.setFillColor(sf::Color(79, 79, 79));

	_slider = sf::RectangleShape(sf::Vector2f(width, width));
	_slider.setFillColor(sf::Color::Red);
	updateSliderSize();
	updateSliderPosition();

	setPosition(x, y);

	_state = ScrollbarState::Idle;
	_func = {};
	_dragOffset = sf::Vector2f(0, 0);

	
}


Scrollbar::~Scrollbar() {

}

void Scrollbar::updateSliderSize() {

	sf::Vector2f size = _rect.getSize();

	size.y = (_slider_size / (float)(_max_value - _min_value)) * size.y;
	_slider.setSize(size);
}

void Scrollbar::updateSliderPosition() {
	sf::Vector2f position = _rect.getPosition();
	position.y += ((_value - _min_value) / (float)(_max_value - _min_value)) * (_rect.getSize().y - _slider.getSize().y);
	_slider.setPosition(position);
}

void Scrollbar::setPosition(float x, float y) {
	_rect.setPosition(x, y);
	_slider.setPosition(x, y);
}

int Scrollbar::getValue() {
	return _value;
}

void Scrollbar::cursorHover() {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void Scrollbar::handleEvent(sf::Event& event) {
	
	if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
		if (ElementGUI_pressed == this) {
			_state = ScrollbarState::Idle;
		}
	}
	else if (_slider.getGlobalBounds().contains(worldMousePosition)) {
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			ElementGUI_pressed = this;
			_state = ScrollbarState::Dragging;
			_dragOffset = worldMousePosition - _slider.getPosition();
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
}

void Scrollbar::draw() {
	window->draw(_rect);
	window->draw(_slider);
}