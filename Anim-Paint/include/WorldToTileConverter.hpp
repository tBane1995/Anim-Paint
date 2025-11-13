#pragma once
#include "SFML/Graphics.hpp"

sf::Vector2i worldToTile(sf::Vector2i p, sf::Vector2i canvasPosition, sf::Vector2i size, float zoom, float zoom_delta);
sf::Vector2i selectionToTile(sf::Vector2i p, sf::Vector2i canvasPosition, sf::Vector2i canvas_size, sf::Vector2i selection_size, sf::Vector2i selection_offset, float zoom, float zoom_delta);
sf::Vector2i worldToTile(sf::Vector2i p, sf::Vector2i canvasPosition, float zoom, float zoom_delta);
sf::Vector2i tileToWorld(sf::Vector2i p, sf::Vector2i canvas_position, float zoom, float zoom_delta);
