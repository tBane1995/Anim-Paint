#pragma once
#include "SFML/Graphics.hpp"

void removeImage(sf::Image& image, sf::IntRect rect, sf::Color alphaColor = sf::Color::Transparent);
void removeImage(sf::Image& image, sf::IntRect rect, sf::Image& mask, sf::Color alphaColor = sf::Color::Transparent);

void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect);
void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect, sf::Color emptyColor = sf::Color::Transparent);
void copyImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, int srcX, int srcY, sf::Image& mask, sf::Color alphaColor);

void pasteImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Color alphaColor = sf::Color::Transparent);
void pasteImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor = sf::Color::Transparent);
