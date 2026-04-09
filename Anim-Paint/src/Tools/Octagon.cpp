#include <SFML/Graphics.hpp>
#include "Tools/Octagon.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

Octagon::Octagon() : ResizableTool() {
}

Octagon::~Octagon() {
}

void Octagon::generateImage() {

	if (_rect.size.x < 1 || _rect.size.y < 1)
		return;

	sf::RenderTexture rtex;
	rtex.resize(sf::Vector2u(_rect.size));
	rtex.clear(sf::Color::Transparent);

    sf::ConvexShape convex;
    convex.setPointCount(8);
    convex.setFillColor(toolbar->_first_color->_color);

	convex.setPoint(0, sf::Vector2f((float)(_rect.size.x)*0.75f, 0));
	convex.setPoint(1, sf::Vector2f((float)(_rect.size.x), (float)(_rect.size.y)*0.25f));
	convex.setPoint(2, sf::Vector2f((float)(_rect.size.x), (float)(_rect.size.y)*0.75f));
	convex.setPoint(3, sf::Vector2f((float)(_rect.size.x)*0.75f, (float)(_rect.size.y)));
	convex.setPoint(4, sf::Vector2f((float)(_rect.size.x)*0.25f, (float)(_rect.size.y)));
	convex.setPoint(5, sf::Vector2f(0, (float)(_rect.size.y) * 0.75f));
	convex.setPoint(6, sf::Vector2f(0, (float)(_rect.size.y) * 0.25f));
	convex.setPoint(7, sf::Vector2f((float)(_rect.size.x) * 0.25f, 0));

	rtex.draw(convex);
	rtex.display();


	_image = std::make_shared<sf::Image>(rtex.getTexture().copyToImage());
}