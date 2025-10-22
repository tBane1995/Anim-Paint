#pragma once
#include "SFML/Graphics.hpp"


void set_rotation(sf::Image& image, float angle, bool set_smooth);
void set_brightness(sf::Image& image, float value);
void set_contrast(sf::Image& image, float value);
void set_saturation(sf::Image& image, float value);
void set_sepia(sf::Image& image, float value);
