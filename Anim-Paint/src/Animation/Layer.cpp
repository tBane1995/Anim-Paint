#include "Animation/Layer.hpp"

Layer::Layer(std::wstring name, sf::Vector2i size) {
	this->name = name;

	image = sf::Image();
	image.create(size.x, size.y, sf::Color::White);
}

Layer::Layer(Layer* layer) {
	this->name = layer->name;
	this->image = layer->image;
}

Layer::~Layer() {

}