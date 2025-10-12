#include "ElementGUI/Checkbox.hpp"
#include "SFML/Graphics.hpp"
#include "Time.hpp"
#include "Window.hpp"
#include "Mouse.hpp"

Checkbox::Checkbox(Texture* texture, Texture* hoverTexture) :
	ElementGUI(),
	_sprite(*texture->_texture)
{

	_value = -1;
	_textures.clear();
	_hoverTextures.clear();

	_textures.push_back(texture);
	_hoverTextures.push_back(hoverTexture);
}

Checkbox::~Checkbox() {

}

void Checkbox::setPosition(sf::Vector2f position) {
	_position = position;
	_sprite.setPosition(_position);
}

void Checkbox::setValue(int value) {
	_value = value;

	_sprite.setTexture(*_textures[_value]->_texture);
}

void Checkbox::addValue(Texture* texture, Texture* hoverTexture) {

	_textures.push_back(texture);
	_hoverTextures.push_back(hoverTexture);
}

void Checkbox::unclick() {
	_state = CheckboxState::Idle;
	_sprite.setTexture(*_textures[_value]->_texture);
}

void Checkbox::hover() {
	_state = CheckboxState::Hover;
	_sprite.setTexture(*_hoverTextures[_value]->_texture);

}

void Checkbox::click() {
	_state = CheckboxState::Pressed;

	_value += 1;
	if (_value >= _textures.size())
		_value = 0;

	_sprite.setTexture(*_hoverTextures[_value]->_texture);
	_clickTime = currentTime;
}

void Checkbox::cursorHover() {

	if (_value > -1) {
		if (_sprite.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}

}

void Checkbox::handleEvent(const sf::Event& event) {
	if (_sprite.getGlobalBounds().contains(worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
		}

	}
}

void Checkbox::update() {

	if (_state == CheckboxState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();
}

void Checkbox::draw() {
	window->draw(_sprite);
}