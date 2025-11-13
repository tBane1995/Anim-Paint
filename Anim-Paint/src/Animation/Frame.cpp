#include "Animation/Frame.hpp"

Frame::Frame() {

}

Frame::~Frame() {}

std::vector<std::shared_ptr<Layer>>& Frame::getLayers() {
	return _layers;
}

std::shared_ptr<Layer> Frame::getLayer(int id) {
	if (id < 0) return nullptr;
	if (id >= _layers.size()) return nullptr;

	return _layers[id];
}

int Frame::getLayersCount() {
	return _layers.size();
}