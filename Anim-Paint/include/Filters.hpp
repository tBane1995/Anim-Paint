#pragma once
#include "SFML/Graphics.hpp"

extern std::string palette_button_shader_source;
extern std::string palette_colors_shader_source;
extern std::string palette_values_shader_source;

void set_rotation(sf::Image& image, int angle, bool set_smooth, sf::Color backgroundColor);
void set_brightness(sf::Image& image, int value);
void set_contrast(sf::Image& image, int value);
void set_saturation(sf::Image& image, int value);
void set_sepia(sf::Image& image, int value);
void set_outline(sf::Image& image, int width, sf::Color backgroundColor, sf::Color outlineColor);
void set_resize(sf::Image& image, int width, int height, sf::Color backgroundColor);
void set_chessboard(sf::Image& image, int tileCount, int transparency, sf::Color firstColor, sf::Color secondColor);
void set_invert_rgb(sf::Image& image);
void set_invert_hsv(sf::Image& image);
void set_hue(sf::Image& image, int value);