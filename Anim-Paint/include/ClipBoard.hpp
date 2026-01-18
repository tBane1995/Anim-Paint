#pragma once
#include "SFML/Graphics.hpp"

bool copyImageToClipboard(sf::Image& image, sf::IntRect rect);
void loadImageFromClipboard(sf::Image& outImage);