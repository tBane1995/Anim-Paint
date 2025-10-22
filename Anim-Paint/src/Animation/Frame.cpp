#include "Animation/Frame.hpp"

Frame::Frame() {
	_layers.clear();

}

Frame::~Frame() {}

std::vector < Layer* >& Frame::getLayers() {
	return _layers;
}