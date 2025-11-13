#include "ElementGUI/Slider.hpp"
#include "ElementGUI/ElementGUI.hpp"
#include "Textures.hpp"
#include "SFML/Graphics.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

Slider::Slider(int min_value, int max_value) : ElementGUI() {
	_min_value = min_value;
	_max_value = max_value;
	_current_value = min_value;

	_barRect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(128, 8));
	_sliderRect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(16, 16));

	_state = SliderState::Idle;
}

Slider::~Slider() {

}

void Slider::setValue(int value) {
	_current_value = value;

	if (_current_value < _min_value)
		_current_value = _min_value;
	else if (_current_value > _max_value)
		_current_value = _max_value;

	_sliderRect.position = getSliderPosition();
}

int Slider::getValue() {
	return _current_value;
}

sf::Vector2i Slider::getSliderPosition() {
	float line_length = _barRect.size.x - 16;

	sf::Vector2i result;
	result.x = _barRect.position.x + float(_current_value - _min_value) * line_length / float(_max_value - _min_value);
	result.y = _barRect.position.y - 4;
	return result;
}

void Slider::setPosition(sf::Vector2i position) {
	_barRect.position = position;
	_sliderRect.position = getSliderPosition();
}

void Slider::cursorHover() {

}

void Slider::handleEvent(const sf::Event& event) {	

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_sliderRect.contains(cursor->_worldMousePosition)) {
			ElementGUI_pressed = this->shared_from_this();
			_offset = cursor->_worldMousePosition - sf::Vector2i(_sliderRect.position);
			_state = SliderState::Changed;

		}
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (ElementGUI_pressed.get() == this) {
			ElementGUI_pressed = nullptr;
			_state = SliderState::Idle;
		}
	}

}

void Slider::update() {
	if (_state == SliderState::Changed) {
		sf::Vector2i newPos = cursor->_worldMousePosition - _offset;
		newPos.y = _barRect.position.y - 4;
		newPos.x = std::clamp(newPos.x, int(_barRect.position.x), int(_barRect.position.x + _barRect.size.x - 16));
		float ratio = float(newPos.x - _barRect.position.x) / float(_barRect.size.x - 16);
		int newValue = static_cast<int>(std::round(ratio * (_max_value - _min_value) + _min_value));
		setValue(newValue);
	}
}



void Slider::draw() {

	sf::Sprite barSprite(*getTexture(L"tex\\slider\\bar.png")->_texture);
	barSprite.setScale({ 128 / 8, 1.0f });
	barSprite.setPosition(sf::Vector2f(_barRect.position));
	window->draw(barSprite);

	sf::Sprite sliderSprite(*getTexture(L"tex\\slider\\slider.png")->_texture);
	sliderSprite.setPosition(sf::Vector2f(_sliderRect.position));
	window->draw(sliderSprite);

}