#include "ElementGUI/Slider.hpp"
#include "ElementGUI/ElementGUI.hpp"
#include "Textures.hpp"
#include "SFML/Graphics.hpp"
#include "Mouse.hpp"
#include "Window.hpp"

Slider::Slider(int min_value, int max_value) : ElementGUI() {
	_min_value = min_value;
	_max_value = max_value;
	_current_value = min_value;

	_bar_texture = getTexture(L"tex\\slider\\bar.png");
	_bar_sprite = sf::Sprite(*_bar_texture->_texture);
	_bar_sprite.setScale(128 / 8, 1.0f);

	_slider_texture = getTexture(L"tex\\slider\\slider.png");
	_slider_sprite = sf::Sprite(*_slider_texture->_texture);

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

	_slider_sprite.setPosition(getSliderPosition());

}

int Slider::getValue() {
	return _current_value;
}

sf::Vector2f Slider::getSliderPosition() {
	float line_length = _bar_sprite.getGlobalBounds().getSize().x - 16;

	sf::Vector2f result;
	result.x = _bar_sprite.getPosition().x + float(_current_value - _min_value) * line_length / float(_max_value - _min_value);
	result.y = _bar_sprite.getPosition().y - 4;

	return result;
}

void Slider::setPosition(sf::Vector2f position) {
	_bar_sprite.setPosition(position);
	_slider_sprite.setPosition(getSliderPosition());
}

void Slider::cursorHover() {

}
void Slider::handleEvent(sf::Event& event) {

	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		if (_slider_sprite.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_pressed = this;
			_offset = worldMousePosition - _slider_sprite.getPosition();
			_state = SliderState::Changed;

		}

	}
	else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
		if (ElementGUI_pressed == this) {
			ElementGUI_pressed = nullptr;
			_state = SliderState::Idle;
		}
	}

}

void Slider::update() {
	if (_state == SliderState::Changed) {
		sf::Vector2f newPos = worldMousePosition - _offset;
		newPos.y = _bar_sprite.getPosition().y - 4;
		newPos.x = std::clamp(newPos.x, _bar_sprite.getPosition().x, _bar_sprite.getPosition().x + _bar_sprite.getGlobalBounds().getSize().x - 16);
		_slider_sprite.setPosition(newPos);

		_current_value = float(_slider_sprite.getPosition().x - _bar_sprite.getPosition().x) / (_bar_sprite.getGlobalBounds().getSize().x - 16) * (float(_max_value - _min_value)) + _min_value;
	}
}



void Slider::draw() {
	window->draw(_bar_sprite);
	window->draw(_slider_sprite);
}