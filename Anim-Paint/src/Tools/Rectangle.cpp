#include <SFML/Graphics.hpp>
#include "Tools/Rectangle.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

Rectangle::Rectangle() : ResizableTool() {
}

Rectangle::~Rectangle() {
}

void Rectangle::generateImage() {

	sf::RenderTexture rtex;
	rtex.resize(sf::Vector2u(_rect.size));
	rtex.clear(sf::Color::Transparent);

    sf::ConvexShape convex;
    convex.setPointCount(4);
    convex.setFillColor(toolbar->_first_color->_color);

	convex.setPoint(0, sf::Vector2f(0, 0));
	convex.setPoint(1, sf::Vector2f((float)(_rect.size.x), 0));
	convex.setPoint(2, sf::Vector2f((float)(_rect.size.x), (float)(_rect.size.y)));
	convex.setPoint(3, sf::Vector2f(0, (float)(_rect.size.y)));

	rtex.draw(convex);
	rtex.display();


	_image = std::make_shared<sf::Image>(rtex.getTexture().copyToImage());
}