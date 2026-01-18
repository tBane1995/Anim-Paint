#include "Animation/Frame.hpp"
#include "Canvas.hpp"

Frame::Frame() {
	
}

Frame::~Frame() {}

std::vector<std::shared_ptr<Layer>>& Frame::getLayers() {
	return _layers;
}

void Frame::addEmptyLayer() {
	_layers.push_back(std::make_shared<Layer>(L"Layer 0", canvas->_size));
}

std::shared_ptr<Layer> Frame::getLayer(int id) {
	if (id < 0) return nullptr;
	if (id >= _layers.size()) return nullptr;

	return _layers[id];
}

int Frame::getLayersCount() {
	return _layers.size();
}

void Frame::addLayer(std::shared_ptr<Layer> layer) {
	_layers.push_back(layer);
}