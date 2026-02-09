#include "Animation/Animation.hpp"
#include <iostream>
#include "Canvas.hpp"

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
	layers.push_back(std::make_shared<Layer>(L"Layer " + std::to_wstring(layers.size()+1), sf::Vector2i(size.x, size.y)));
	_frames.push_back(frame);
	
	//std::wcout << "added Empty Frame\n";
	//std::wcout << L"frames: " << animation->getFrames().size() << L"\n";
	//std::wcout << L"layers: " << animation->getLayers().size() << L"\n";
}

void Animation::addFrame(std::shared_ptr<Frame> frame) {
	_frames.push_back(frame);
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

	static std::vector<std::shared_ptr<Layer>> emptyLayers;

	if (_frames.empty())
		return emptyLayers;

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
	return (int)_frames.size();
}

int Animation::getLayersCount() {
	// return current Layers size
	return (int)getCurrentFrame()->_layers.size();
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
	_currentFrame = getFramesCount() - 1;
}

void Animation::nextFrame() {
	_currentFrame = (_currentFrame + 1) % getFramesCount();
}

void Animation::prevFrame() {

	_currentFrame = (_currentFrame - 1 + getFramesCount()) % getFramesCount();
}

void Animation::firstLayer() {
	_currentLayer = 0;
}

void Animation::lastLayer() {
	if (getCurrentFrame() == nullptr) {
		_currentLayer = 0;
		return;
	}
	_currentLayer = (int)getCurrentFrame()->_layers.size() - 1;
}

void Animation::addFrame() {
	std::shared_ptr<Frame> newFrame = std::make_shared<Frame>();
	newFrame->addEmptyLayer();
	_frames.insert(_frames.begin() + getCurrentFrameID() + 1, newFrame);
}

void Animation::subFrame() {
	if(getFramesCount() > 1)
		_frames.erase(_frames.begin() + getCurrentFrameID());
}

void Animation::moveBackFrame() {
	std::shared_ptr<Frame> frame = getCurrentFrame();
	_frames.erase(_frames.begin() + getCurrentFrameID());

	int nextPos = (getCurrentFrameID() - 1 + getFramesCount()) % getFramesCount();
	_frames.insert(_frames.begin() + nextPos, frame);
}

void Animation::moveNextFrame() {
	std::shared_ptr<Frame> frame = getCurrentFrame();
	_frames.erase(_frames.begin() + getCurrentFrameID());

	int nextPos = (getCurrentFrameID() + 1) % getFramesCount();
	_frames.insert(_frames.begin() + nextPos, frame);
}

////////////////////////////////////////////////////////////////////////

std::vector<std::shared_ptr<Animation>> animations;
int currentAnimationId;
int maxAnimationsCount = 16;
int maxFramesCount = 16;

////////////////////////////////////////////////////////////////////////

std::shared_ptr<Animation> getCurrentAnimation() {
	return animations[currentAnimationId];
}

int getCurrentAnimationId() {
	return currentAnimationId;
}

int getAnimationsCount() {
	return (int)animations.size();
}

std::shared_ptr<Animation> getAnimation(int id) {
	return animations[id];
}

void nextAnimation() {
	currentAnimationId = (currentAnimationId + 1) % getAnimationsCount();
}

void prevAnimation() {
	currentAnimationId = (currentAnimationId - 1 + getAnimationsCount()) % getAnimationsCount();

}

void firstAnimation() {

	currentAnimationId = 0;
}

void lastAnimation() {

	currentAnimationId = getAnimationsCount() - 1;
}

void addAnimation() {
	std::shared_ptr<Animation> newAnim = std::make_shared<Animation>();
	newAnim->addEmptyFrame(canvas->_size);
	animations.insert(animations.begin() + currentAnimationId + 1, newAnim);
}

void deleteAnimation() {
	if(getAnimationsCount() > 1)
		animations.erase(animations.begin() + currentAnimationId);
}

void moveBackAnimation() {
	std::shared_ptr<Animation> anim = getAnimation(currentAnimationId);
	animations.erase(animations.begin() + currentAnimationId);

	int nextPos = (currentAnimationId - 1 + getAnimationsCount()) % getAnimationsCount();
	animations.insert(animations.begin() + nextPos, anim);
}

void moveNextAnimation() {
	std::shared_ptr<Animation> anim = getAnimation(currentAnimationId);
	animations.erase(animations.begin() + currentAnimationId);

	int nextPos = (currentAnimationId + 1) % getAnimationsCount();
	animations.insert(animations.begin() + nextPos, anim);
}

