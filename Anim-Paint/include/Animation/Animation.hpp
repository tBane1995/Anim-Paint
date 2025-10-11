#pragma once
#include "Frame.hpp"

class Animation {
public:
	std::vector < Frame* > _frames;
	int _currentFrame;
	int _currentLayer;

	Animation();
	~Animation();

	Frame* getCurrentFrame();
	std::vector < Layer* > getLayers();
	Layer* getCurrentLayer();
	std::vector < Frame* > getFrames();

	Frame* getFrame(int id);
	Layer* getLayer(int id);

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

extern Animation* animation;
