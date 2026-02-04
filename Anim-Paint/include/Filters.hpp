#pragma once
#include "SFML/Graphics.hpp"

extern std::string palette_button_shader_source;
extern std::string palette_colors_shader_source;
extern std::string palette_values_shader_source;

extern std::string rotation_shader_source;
extern std::string brightness_shader_source;
extern std::string contrast_shader_source;
extern std::string saturation_shader_source;
extern std::string sepia_shader_source;
extern std::string outline_shader_source;
extern std::string chessboard_shader_source;

void set_rotation(sf::Image& image, float angle, bool set_smooth);
void set_brightness(sf::Image& image, float value);
void set_contrast(sf::Image& image, float value);
void set_saturation(sf::Image& image, float value);
void set_sepia(sf::Image& image, float value);
void set_outline(sf::Image& image, int width, sf::Color backgroundColor, sf::Color outlineColor);
void set_resize(sf::Image& image, int width, int height);
void set_chessboard(sf::Image& image, int tileCount, int transparency, sf::Color firstColor, sf::Color secondColor);
