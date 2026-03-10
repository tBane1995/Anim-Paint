#include "Components/Toolbar/LargeColorButton.hpp"
#include "Window.hpp"

LargeColorButton::LargeColorButton(sf::Color color) : ButtonWithSprite(getTexture(L"tex\\tools\\frame_large.png"), getTexture(L"tex\\tools\\frame_large_hover.png")) {
	_color = color;
}

LargeColorButton::~LargeColorButton() {

}

void LargeColorButton::setPosition(sf::Vector2i position) {
	ButtonWithSprite::setPosition(position);
}

void LargeColorButton::setColor(sf::Color color) {
	_color = color;
}

void LargeColorButton::draw() {
	ButtonWithSprite::draw();

	sf::RectangleShape color_rect(sf::Vector2f(40, 40));
	color_rect.setFillColor(_color);
	color_rect.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(4, 4));
	window->draw(color_rect);

}