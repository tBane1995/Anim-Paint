#include <SFML/Graphics.hpp>
#include "Tools/HexagonPointTop.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

HexagonPointTop::HexagonPointTop() : ResizableTool() {
}

HexagonPointTop::~HexagonPointTop() {
}

void HexagonPointTop::generateImage() {

	sf::RenderTexture rtex;
	rtex.resize(sf::Vector2u(_rect.size));
	rtex.clear(sf::Color::Transparent);

	// triangle
	sf::ConvexShape convex;
	convex.setFillColor(toolbar->_first_color->_color);
	convex.setPointCount(6);

	// point-top hexagon
	convex.setPoint(0, sf::Vector2f(0.5f, 0.f));
	convex.setPoint(1, sf::Vector2f(1.f, 0.25f));
	convex.setPoint(2, sf::Vector2f(1.f, 0.75f));
	convex.setPoint(3, sf::Vector2f(0.5f, 1.f));
	convex.setPoint(4, sf::Vector2f(0.f, 0.75f));
	convex.setPoint(5, sf::Vector2f(0.f, 0.25f));
	
	convex.setScale(sf::Vector2f(_rect.size.x, _rect.size.y));

	rtex.draw(convex);
	rtex.display();

	_image = std::make_shared<sf::Image>(rtex.getTexture().copyToImage());
}