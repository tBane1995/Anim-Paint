#include "Animation/Animation.hpp"

Animation::Animation() {

	_currentFrame = 0;
	_currentLayer = 0;

	int size = 64;

	Frame* frame;

	{
		frame = new Frame();
		std::vector < Layer* >& layers = frame->getLayers();
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		_frames.push_back(frame);
	}

	{
		frame = new Frame();
		std::vector < Layer* >& layers = frame->getLayers();
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		layers.push_back(new Layer(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size, size)));
		_frames.push_back(frame);
	}

	lastLayer();

}

Animation::~Animation() {

}

Frame* Animation::getCurrentFrame() {
	// return current Frame
	if (_frames.empty())
		return nullptr;

	return _frames[_currentFrame];
}

std::vector < Layer* > Animation::getLayers() {
	// return current Layers
	if (_frames.empty())
		return std::vector < Layer* >();

	return getCurrentFrame()->getLayers();
}

Layer* Animation::getCurrentLayer() {
	// return current Layer
	if (_frames.empty())
		return nullptr;

	return getCurrentFrame()->getLayers()[_currentLayer];
}

std::vector < Frame* > Animation::getFrames() {
	return _frames;
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
	return _currentFrame;
}

int Animation::getCurrentLayerID() {
	// return current Layer ID
	return _currentLayer;
}

int Animation::getFramesSize() {
	// return Frames size
	return _frames.size();
}

int Animation::getLayersSize() {
	// return current Layers size
	return getCurrentFrame()->_layers.size();
}

void Animation::setCurrentFrameID(int id) {
	// set current Frame ID
	_currentFrame = id;
}

void Animation::setCurrentLayerID(int id) {
	// set current Layer ID
	_currentLayer = id;
}

void Animation::firstFrame() {
	_currentFrame = 0;
}

void Animation::lastFrame() {
	_currentFrame = _frames.size() - 1;
}

void Animation::nextFrame() {
	_currentFrame += 1;

	if (_currentFrame >= _frames.size())
		_currentFrame = 0;
}

void Animation::prevFrame() {
	_currentFrame -= 1;

	if (_currentFrame < 0)
		_currentFrame = _frames.size() - 1;
}

void Animation::firstLayer() {
	_currentLayer = 0;
}

void Animation::lastLayer() {
	_currentLayer = getCurrentFrame()->_layers.size() - 1;
}

Animation* animation = nullptr;
