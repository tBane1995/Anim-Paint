#include "Controls/OptionWithIcon.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"


OptionWithIcon::OptionWithIcon(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, std::wstring shortcut, sf::Vector2i position) : Option(text, shortcut, position) {
	_icon = texture;
	_icon_hover = hoverTexture;
}

OptionWithIcon::~OptionWithIcon() {

}

void OptionWithIcon::draw() {
	Option::draw();
	std::shared_ptr<Texture> texture;

	switch (_state) {
		case ButtonState::Idle:
			texture = _icon;
			break;
		case ButtonState::Hover:
			texture = _icon_hover;
			break;
		case ButtonState::Pressed:
			texture = _icon_hover;
			break;
		default:
			texture = _icon;
			break;
	};

	sf::Sprite sprite(*texture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);
}