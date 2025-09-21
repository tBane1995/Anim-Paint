#include "Animation/Animation.hpp"

Animation::Animation() {

	currentFrame = 0;
	currentLayer = 0;

	int size = 64;

	Frame* frame;

	{
		frame = new Frame();
		std::vector < Layer* >& layers = frame->getLayers();
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		frames.push_back(frame);
	}

	{
		frame = new Frame();
		std::vector < Layer* >& layers = frame->getLayers();
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		frames.push_back(frame);
	}

	lastLayer();

}

Animation::~Animation() {

}

Frame* Animation::getCurrentFrame() {
	// return current Frame
	if (frames.empty())
		return nullptr;

	return frames[currentFrame];
}

std::vector < Layer* > Animation::getLayers() {
	// return current Layers
	if (frames.empty())
		return std::vector < Layer* >();

	return getCurrentFrame()->getLayers();
}

Layer* Animation::getCurrentLayer() {
	// return current Layer
	if (frames.empty())
		return nullptr;

	return getCurrentFrame()->getLayers()[currentLayer];
}

std::vector < Frame* > Animation::getFrames() {
	return frames;
}

Layer* Animation::getLayer(int id) {
	// return Layer
	if (id < 0)
		return nullptr;

	else if (id >= getCurrentFrame()->getLayers().size())
		return nullptr;

	else
		return getCurrentFrame()->getLayers()[id];
}

int Animation::getCurrentFrameID() {
	// return current Frame ID
	return currentFrame;
}

int Animation::getCurrentLayerID() {
	// return current Layer ID
	return currentLayer;
}

int Animation::getFramesSize() {
	// return Frames size
	return frames.size();
}

int Animation::getLayersSize() {
	// return current Layers size
	return getCurrentFrame()->layers.size();
}

void Animation::setCurrentFrameID(int id) {
	// set current Frame ID
	currentFrame = id;
}

void Animation::setCurrentLayerID(int id) {
	// set current Layer ID
	currentLayer = id;
}

void Animation::firstFrame() {
	currentFrame = 0;
}

void Animation::lastFrame() {
	currentFrame = frames.size() - 1;
}

void Animation::nextFrame() {
	currentFrame += 1;

	if (currentFrame >= frames.size())
		currentFrame = 0;
}

void Animation::prevFrame() {
	currentFrame -= 1;

	if (currentFrame < 0)
		currentFrame = frames.size() - 1;
}

void Animation::firstLayer() {
	currentLayer = 0;
}

void Animation::lastLayer() {
	currentLayer = getCurrentFrame()->layers.size() - 1;
}

Animation* animation = nullptr;
