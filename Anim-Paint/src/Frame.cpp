#include "Frame.hpp"

Frame::Frame() {
	layers.clear();

}

Frame::~Frame() {}

std::vector < Layer* >& Frame::getLayers() {
	return layers;
}