#pragma once
#include <SFML/Graphics.hpp>

class Layer {
public:
	std::wstring _name;
	sf::Image _image;

	Layer(std::wstring name, sf::Vector2i size);
	Layer(std::wstring name, sf::Image image);
	Layer(Layer* layer);
	~Layer();
};
