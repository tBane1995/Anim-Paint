#include <SFML/Graphics.hpp>
#include "Tools/HexagonFlatTop.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

HexagonFlatTop::HexagonFlatTop() : ResizableTool() {
}

HexagonFlatTop::~HexagonFlatTop() {
}

void HexagonFlatTop::generateImage() {

	sf::RenderTexture rtex;
	rtex.resize(sf::Vector2u(_rect.size));
	rtex.clear(sf::Color::Transparent);

	// triangle
	sf::ConvexShape convex;
	convex.setFillColor(toolbar->_first_color->_color);
	convex.setPointCount(6);

	float dx = (float)(_rect.size.x) / 4.f;

	convex.setPoint(0, sf::Vector2f(dx, 0));
	convex.setPoint(1, sf::Vector2f(_rect.size.x-dx, 0));
	convex.setPoint(2, sf::Vector2f(_rect.size.x, _rect.size.y/2));
	convex.setPoint(3, sf::Vector2f(_rect.size.x-dx, _rect.size.y));
	convex.setPoint(4, sf::Vector2f(dx, _rect.size.y));
	convex.setPoint(5, sf::Vector2f(0, _rect.size.y/2));
	
	rtex.draw(convex);
	rtex.display();

	rtex.draw(convex);
	rtex.display();

	_image = std::make_shared<sf::Image>(rtex.getTexture().copyToImage());
}