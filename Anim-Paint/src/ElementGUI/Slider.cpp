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

	_bar_sprite = new sf::Sprite(*getTexture(L"tex\\slider\\bar.png")->_texture);
	_slider_sprite = new sf::Sprite(*getTexture(L"tex\\slider\\slider.png")->_texture);

	_bar_sprite->setScale({ 128 / 8, 1.0f });

	_state = SliderState::Idle;
}

Slider::~Slider() {
	delete _bar_sprite;
	delete _slider_sprite;

}

void Slider::setValue(int value) {
	_current_value = value;

	if (_current_value < _min_value)
		_current_value = _min_value;
	else if (_current_value > _max_value)
		_current_value = _max_value;

	_slider_sprite->setPosition(getSliderPosition());

}

int Slider::getValue() {
	return _current_value;
}

sf::Vector2f Slider::getSliderPosition() {
	float line_length = _bar_sprite->getGlobalBounds().size.x - 16;

	sf::Vector2f result;
	result.x = _bar_sprite->getPosition().x + float(_current_value - _min_value) * line_length / float(_max_value - _min_value);
	result.y = _bar_sprite->getPosition().y - 4;

	return result;
}

void Slider::setPosition(sf::Vector2f position) {
	_bar_sprite->setPosition(position);
	_slider_sprite->setPosition(getSliderPosition());
}

void Slider::cursorHover() {

}
void Slider::handleEvent(const sf::Event& event) {	

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_slider_sprite->getGlobalBounds().contains(cursor->_worldMousePosition)) {
			ElementGUI_pressed = this;
			_offset = cursor->_worldMousePosition - _slider_sprite->getPosition();
			_state = SliderState::Changed;

		}
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (ElementGUI_pressed == this) {
			ElementGUI_pressed = nullptr;
			_state = SliderState::Idle;
		}
	}

}

void Slider::update() {
	if (_state == SliderState::Changed) {
		sf::Vector2f newPos = cursor->_worldMousePosition - _offset;
		newPos.y = _bar_sprite->getPosition().y - 4;
		newPos.x = std::clamp(newPos.x, _bar_sprite->getPosition().x, _bar_sprite->getPosition().x + _bar_sprite->getGlobalBounds().size.x - 16);
		_slider_sprite->setPosition(newPos);

		_current_value = float(_slider_sprite->getPosition().x - _bar_sprite->getPosition().x) / (_bar_sprite->getGlobalBounds().size.x - 16) * (float(_max_value - _min_value)) + _min_value;
	}
}



void Slider::draw() {
	window->draw(*_bar_sprite);
	window->draw(*_slider_sprite);
}