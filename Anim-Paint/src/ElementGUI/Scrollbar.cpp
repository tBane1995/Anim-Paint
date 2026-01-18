#include "ElementGUI/Scrollbar.hpp"
#include "Window.hpp"
#include "Cursor.hpp"
#include <iostream>

Scrollbar::Scrollbar(int x, int y, int width, int height, int min_value, int max_value, int slider_size, int value) {
	
	
	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(width, height));
	_slider = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(width, width));

	_min_value = min_value;
	_max_value = max_value;
	_slider_size = slider_size;
	_value = value;

	updateSliderSize();
	setPosition(sf::Vector2i(x,y));

	_state = ScrollbarState::Idle;
	_func = {};
	_dragOffset = sf::Vector2i(0, 0);

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

void Scrollbar::setScrollArea(std::shared_ptr<sf::IntRect> rect, float scrollStep) {
	_scrollArea = rect;
	_scrollStep = scrollStep;
}

void Scrollbar::updateSliderSize() {
	sf::Vector2i slider = _rect.size;
	int range = _max_value - _min_value;

	if (range <= 0) {										// treść mieści się w oknie
		_slider.size = sf::Vector2i(slider.x, slider.y);    // kciuk = pełen tor
		return;
	}

	float sizeY = slider.y * ((float)_slider_size / ((float)range + (float)_slider_size));

	_slider.size = sf::Vector2i(slider.x, sizeY);
}

void Scrollbar::updateSliderPosition() {
	int range = _max_value - _min_value;

	sf::Vector2i pos = _rect.position;

	if (range <= 0) {                // nic do przewijania
		_slider.position = pos;
		return;
	}

	pos.y += ((float)(_value - _min_value) / (float)range) * float(_rect.size.y - _slider.size.y);

	_slider.position = pos;
}

void Scrollbar::setPosition(sf::Vector2i position) {
	_rect.position = position;
	updateSliderPosition();
}

int Scrollbar::getValue() {
	return _value;
}

void Scrollbar::cursorHover() {
	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}
}

void Scrollbar::handleEvent(const sf::Event& event) {

	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (ElementGUI_pressed.get() == this) {
			_state = ScrollbarState::Idle;
		}
	}
	else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_slider.contains(cursor->_worldMousePosition)) {
			ElementGUI_pressed = this->shared_from_this();
			if (_slider.size.y < _rect.size.y) {
				_state = ScrollbarState::Dragging;
				_dragOffset = cursor->_worldMousePosition - _slider.position;
			}
		}
		
	}
	else if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>(); mws) {
		if (_rect.contains(cursor->_worldMousePosition) || _scrollArea->contains(cursor->_worldMousePosition)) {
			_state = ScrollbarState::Scrolled;
			_deltaScroll = -mws->delta;
		}
	}
	

}

void Scrollbar::update() {
	if (_state == ScrollbarState::Dragging) {
		sf::Vector2f position = sf::Vector2f(_slider.position.x, cursor->_worldMousePosition.y - _dragOffset.y);
		position.y = std::clamp(position.y, float(_rect.position.y), float(_rect.position.y + _rect.size.y - _slider.size.y));
		int val = (int)(((position.y - _rect.position.y) / (_rect.size.y - _slider.size.y)) * (_max_value - _min_value)) + _min_value;

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
	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setPosition(sf::Vector2f(_rect.position));
	rect.setFillColor(sf::Color(79, 79, 79));
	window->draw(rect);

	sf::RectangleShape slider(sf::Vector2f(_slider.size));
	slider.setPosition(sf::Vector2f(_slider.position));
	slider.setFillColor(sf::Color(47, 47, 47));
	window->draw(slider);

}