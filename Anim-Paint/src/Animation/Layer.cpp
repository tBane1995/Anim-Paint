#include "Animation/Layer.hpp"

Layer::Layer(std::wstring name, sf::Vector2i size) {
	_name = name;

	_image = sf::Image();
	_image.create(size.x, size.y, sf::Color::White);
}

Layer::Layer(Layer* layer) {
	_name = layer->_name;
	_image = layer->_image;
}

Layer::~Layer() {

}