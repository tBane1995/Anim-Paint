#include "Animation/Frame.hpp"

Frame::Frame() {
	_layers.clear();

}

Frame::~Frame() {}

std::vector < Layer* >& Frame::getLayers() {
	return _layers;
}

Layer* Frame::getLayer(int id) {
	if (id < 0) return nullptr;
	if (id >= _layers.size()) return nullptr;

	return _layers[id];
}