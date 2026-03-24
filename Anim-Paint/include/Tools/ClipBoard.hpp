#pragma once
#include "SFML/Graphics.hpp"

bool copyImageToClipboard(sf::Image& image, sf::IntRect rect);
std::shared_ptr<sf::Image> loadImageFromClipboard();