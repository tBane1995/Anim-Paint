#include "ElementGUI/Checkbox.hpp"
#include "SFML/Graphics.hpp"
#include "Time.hpp"
#include "Window.hpp"
#include "Mouse.hpp"

Checkbox::Checkbox() : ElementGUI() {
	value = -1;
	textures.clear();
	hoverTextures.clear();
}

Checkbox::~Checkbox() {

}

void Checkbox::setPosition(sf::Vector2f position) {
	this->position = position;
	sprite.setPosition(position);
}

void Checkbox::setValue(int value) {
	this->value = value;

	sprite.setTexture(*textures[value]->texture);
}

void Checkbox::addValue(Texture* texture, Texture* hoverTexture) {

	textures.push_back(texture);
	hoverTextures.push_back(hoverTexture);
}

void Checkbox::unclick() {
	state = CheckboxState::Idle;
	sprite.setTexture(*textures[value]->texture);
}

void Checkbox::hover() {
	state = CheckboxState::Hover;
	sprite.setTexture(*hoverTextures[value]->texture);

}

void Checkbox::click() {
	state = CheckboxState::Pressed;

	value += 1;
	if (value >= textures.size())
		value = 0;

	sprite.setTexture(*hoverTextures[value]->texture);
	clickTime = currentTime;
}

void Checkbox::cursorHover() {

	if (value > -1) {
		if (sprite.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}

}

void Checkbox::handleEvent(sf::Event& event) {
	if (sprite.getGlobalBounds().contains(worldMousePosition)) {
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			ElementGUI_pressed = this;
		}
		else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
		}

	}
}

void Checkbox::update() {

	if (state == CheckboxState::Pressed) {
		if ((currentTime - clickTime).asSeconds() > 0.05f) {
			if (onclick_func) {
				onclick_func();
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
	window->draw(sprite);
}