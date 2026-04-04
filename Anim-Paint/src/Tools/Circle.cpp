#include <SFML/Graphics.hpp>
#include "Tools/Circle.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

Circle::Circle() : ResizableTool() {
}

Circle::~Circle() {
}

void Circle::generateImage() {

	sf::RenderTexture rtex;
	rtex.resize(sf::Vector2u(_rect.size));
	rtex.clear(sf::Color::Transparent);

	float radius = 1;

	sf::CircleShape circle(radius);
	circle.setFillColor(toolbar->_first_color->_color);
	circle.setOrigin(sf::Vector2f(radius, radius));
	circle.setPosition(sf::Vector2f(_rect.size.x / 2.f, _rect.size.y / 2.f));

	float sx = _rect.size.x / 2.f;
	float sy = _rect.size.y / 2.f;
	circle.setScale(sf::Vector2f(sx, sy));

	rtex.draw(circle);
	rtex.display();

	_image = std::make_shared<sf::Image>(rtex.getTexture().copyToImage());
}