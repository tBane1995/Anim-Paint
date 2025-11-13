#include "ElementGUI/Checkbox.hpp"
#include "SFML/Graphics.hpp"
#include "Time.hpp"
#include "Window.hpp"
#include "Cursor.hpp"

Checkbox::Checkbox(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture) : ElementGUI() {

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32));

	_value = -1;
	_textures.clear();
	_hoverTextures.clear();

	_textures.push_back(texture);
	_hoverTextures.push_back(hoverTexture);

	_state = CheckboxState::Idle;
}

Checkbox::~Checkbox() {
}

void Checkbox::setPosition(sf::Vector2i position) {
	_rect.position = position;
}

void Checkbox::setValue(int value) {
	_value = value;
}

void Checkbox::addValue(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture) {

	_textures.push_back(texture);
	_hoverTextures.push_back(hoverTexture);
}

void Checkbox::unclick() {
	_state = CheckboxState::Idle;
}

void Checkbox::hover() {
	_state = CheckboxState::Hover;

}

void Checkbox::click() {
	_state = CheckboxState::Pressed;

	_value += 1;
	if (_value >= _textures.size())
		_value = 0;

	_clickTime = currentTime;
}

void Checkbox::cursorHover() {

	if (_value > -1) {
		if (_rect.contains(cursor->_worldMousePosition)) {
			ElementGUI_hovered = this->shared_from_this();
		}
	}

}

void Checkbox::handleEvent(const sf::Event& event) {
	if (_rect.contains(cursor->_worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed.get() == this) {
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
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();
}

void Checkbox::draw() {	
	sf::Sprite sprite(*_textures[_value]->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);
}