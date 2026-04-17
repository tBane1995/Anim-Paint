#include "Components/Toolbar/ColorButton.hpp"
#include "Window.hpp"

ColorButton::ColorButton(sf::Color color) : ButtonWithSprite(getTexture(L"tex\\toolbar\\frame.png"), getTexture(L"tex\\toolbar\\frame_hover.png"), getTexture(L"tex\\toolbar\\frame_hover.png")) {
	_color = color;

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32));
}

ColorButton::~ColorButton() {

}

void ColorButton::draw() {
	ButtonWithSprite::draw();
	sf::RectangleShape rect(sf::Vector2f(24,24));
	rect.setFillColor(_color);
	rect.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(4, 4));
	window->draw(rect);

}
