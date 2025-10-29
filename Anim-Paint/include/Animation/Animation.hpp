#pragma once
#include "Frame.hpp"

class Animation {
public:
	std::vector<std::shared_ptr<Frame>> _frames;
	int _currentFrame;
	int _currentLayer;

	Animation();
	~Animation();

	void addEmptyFrame(sf::Vector2i size);

	std::shared_ptr<Frame> getCurrentFrame();
	std::vector<std::shared_ptr<Layer>>& getLayers();
	std::shared_ptr<Layer> getCurrentLayer();
	std::vector<std::shared_ptr<Frame>> getFrames();

	std::shared_ptr<Frame> getFrame(int id);
	std::shared_ptr<Layer> getLayer(int id);

	int getCurrentFrameID();
	int getCurrentLayerID();

	int getFramesCount();
	int getLayersCount();

	void setCurrentFrameID(int id);
	void setCurrentLayerID(int id);
	void firstFrame();
	void lastFrame();
	void nextFrame();
	void prevFrame();
	void firstLayer();
	void lastLayer();
};

extern std::shared_ptr<Animation> animation;
