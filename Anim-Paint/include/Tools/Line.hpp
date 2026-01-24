#pragma once
#include <SFML/Graphics.hpp>

std::vector<sf::Vector2i> getPointsFromLine(sf::Vector2i start, sf::Vector2i end);
bool lineOutOfBounds(sf::Vector2i start, sf::Vector2i end, sf::Vector2i size);