#include "Animation/Layer.hpp"
#include "DebugLog.hpp"

Layer::Layer(std::wstring name, sf::Vector2i size) {
	_name = name;

	_image = sf::Image();
	_image.resize(sf::Vector2u(size), sf::Color::White);
}

Layer::Layer(std::wstring name, sf::Image image) {
	_name = name;
	_image = image;

}

Layer::Layer(std::shared_ptr<Layer> layer) {
	_name = layer->_name;

	_image = sf::Image();
	_image.resize(layer->_image.getSize());

	if (!_image.copy(layer->_image, sf::Vector2u(0, 0))) {
		DebugError(L"Failed to copy Image in Layer constructor.");
		exit(0);
	}
	
}

Layer::~Layer() {

}