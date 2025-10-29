#include "Animation/Animation.hpp"
#include <iostream>

Animation::Animation() {

	_currentFrame = 0;
	_currentLayer = 0;

	lastLayer();

}

Animation::~Animation() {

}

void Animation::addEmptyFrame(sf::Vector2i size) {

	//std::wcout << L"Adding Empty Frame of size: " << size.x << L"," << size.y << L"\n";
	
	std::shared_ptr<Frame> frame = std::make_shared<Frame>();
	std::vector<std::shared_ptr<Layer>>& layers = frame->getLayers();
	layers.push_back(std::make_shared<Layer>(L"Layer " + std::to_wstring(layers.size()), sf::Vector2i(size.x, size.y)));
	_frames.push_back(frame);
	
	//std::wcout << "added Empty Frame\n";
	//std::wcout << L"frames: " << animation->getFrames().size() << L"\n";
	//std::wcout << L"layers: " << animation->getLayers().size() << L"\n";
}

std::shared_ptr<Frame> Animation::getCurrentFrame() {
	// return current Frame
	if (_frames.empty())
		return nullptr;

	if (_currentFrame == -1)
		return nullptr;

	return _frames[_currentFrame];
}

std::vector<std::shared_ptr<Layer>>& Animation::getLayers() {
	// return current Layers
	if (_frames.empty())
		return std::vector<std::shared_ptr<Layer>>();

	return getCurrentFrame()->getLayers();
}

std::shared_ptr<Layer> Animation::getCurrentLayer() {
	// return current Layer
	if (_frames.empty())
		return nullptr;

	if (_currentFrame == -1)
		return nullptr;

	if (_currentLayer == -1)
		return nullptr;

	return getCurrentFrame()->getLayers()[_currentLayer];
}

std::vector<std::shared_ptr<Frame>> Animation::getFrames() {
	return _frames;
}

std::shared_ptr<Frame> Animation::getFrame(int id) {
	// return Frame
	return _frames[id];
}

std::shared_ptr<Layer> Animation::getLayer(int id) {
	// return Layer
	if (id < 0)
		return nullptr;
	else if (getCurrentFrame() == nullptr)
		return nullptr;
	else if (getCurrentFrame()->getLayers().empty())
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

int Animation::getFramesCount() {
	// return Frames size
	return _frames.size();
}

int Animation::getLayersCount() {
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
	if (getCurrentFrame() == nullptr) {
		_currentLayer = 0;
		return;
	}
	_currentLayer = getCurrentFrame()->_layers.size() - 1;
}

std::shared_ptr<Animation> animation;
