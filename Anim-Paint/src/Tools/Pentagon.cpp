#include <SFML/Graphics.hpp>
#include "Tools/Pentagon.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

Pentagon::Pentagon() : ResizableTool() {
}

Pentagon::~Pentagon() {
}

void Pentagon::generateImage() {

    if (_rect.size.x < 1 || _rect.size.y < 1)
        return;

	sf::RenderTexture rtex;
	rtex.resize(sf::Vector2u(_rect.size));
	rtex.clear(sf::Color::Transparent);

    sf::ConvexShape convex;
    convex.setPointCount(5);
    convex.setFillColor(sf::Color::Black);

    float radius = std::min(_rect.size.x, _rect.size.y) / 2.f;
    float cx = 0.f;
    float cy = 0.f;

    float angleOffset = -90.f * 3.14159265f / 180.f;

    for (int i = 0; i < 5; i++) {
        float angle = angleOffset + float(i) * 72.f * 3.14159265f / 180.f;
        float x = cx + radius * std::cos(angle);
        float y = cy + radius * std::sin(angle);
        convex.setPoint(i, { x, y });
    }

    float minX = convex.getPoint(0).x, maxX = convex.getPoint(0).x;
    float minY = convex.getPoint(0).y, maxY = convex.getPoint(0).y;

    for (int i = 1; i < 5; i++) {
        sf::Vector2f p = convex.getPoint(i);
        if (p.x < minX) minX = p.x;
        if (p.x > maxX) maxX = p.x;
        if (p.y < minY) minY = p.y;
        if (p.y > maxY) maxY = p.y;
    }

    float bboxWidth = maxX - minX;
    float bboxHeight = maxY - minY;

    for (int i = 0; i < 5; i++) {
        sf::Vector2f p = convex.getPoint(i);
        float x = (p.x - minX) / bboxWidth * (float)(_rect.size.x);
        float y = (p.y - minY) / bboxHeight * (float)(_rect.size.y);
        convex.setPoint(i, { x, y });
    }

	rtex.draw(convex);
	rtex.display();


	_image = std::make_shared<sf::Image>(rtex.getTexture().copyToImage());
}