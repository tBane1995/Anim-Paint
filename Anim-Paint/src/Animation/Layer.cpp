#include "Animation/Layer.hpp"

Layer::Layer(std::wstring name, sf::Vector2i size) {
	_name = name;

	_image = sf::Image();
	_image.resize(sf::Vector2u(size), sf::Color::White);
}

Layer::Layer(std::wstring name, sf::Image image) {
	_name = name;
	_image = image;

}

Layer::Layer(Layer* layer) {
	_name = layer->_name;
	_image = layer->_image;
}

Layer::~Layer() {

}