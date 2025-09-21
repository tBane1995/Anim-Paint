#pragma once
#include <SFML/Graphics.hpp>

class Layer {
public:
	std::wstring name;
	sf::Image image;

	Layer(std::wstring name, sf::Vector2i size);
	Layer(Layer* layer);
	~Layer();
};
