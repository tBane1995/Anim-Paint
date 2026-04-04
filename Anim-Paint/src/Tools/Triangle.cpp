#include <SFML/Graphics.hpp>
#include "Tools/Triangle.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

Triangle::Triangle() : ResizableTool() {
}

Triangle::~Triangle() {
}

void Triangle::generateImage() {

	sf::RenderTexture rtex;
	rtex.resize(sf::Vector2u(_rect.size));
	rtex.clear(sf::Color::Transparent);

	// triangle
	sf::ConvexShape convex;
	convex.setFillColor(toolbar->_first_color->_color);
	convex.setPointCount(3);
	convex.setPoint(0, sf::Vector2f(_rect.size.x / 2 + _rect.size.x%2, 0));
	convex.setPoint(1, sf::Vector2f(_rect.size.x, _rect.size.y));
	convex.setPoint(2, sf::Vector2f(0, _rect.size.y));
	
	rtex.draw(convex);
	rtex.display();

	//float sx = _rect.size.x / 2.f;
	//float sy = _rect.size.y / 2.f;
	//circle.setScale(sf::Vector2f(sx, sy));

	rtex.draw(convex);
	rtex.display();

	_image = std::make_shared<sf::Image>(rtex.getTexture().copyToImage());
}