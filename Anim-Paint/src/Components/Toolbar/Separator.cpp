#include "Components/Toolbar/Separator.hpp"
#include "Theme.hpp"
#include "Window.hpp"

Separator::Separator() {
	_rect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(2, tools_separator_height));

}

Separator::~Separator() {}

void Separator::setPosition(sf::Vector2i position) {
	_rect.position = (position + sf::Vector2i(0, (tools_height - tools_separator_height) / 2));
}

sf::Vector2i Separator::getSize() {
	return _rect.size;
}

void Separator::draw() {
	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setFillColor(tools_separator_color);
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);
}
