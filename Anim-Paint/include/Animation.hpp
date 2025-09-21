#pragma once
#include "Frame.hpp"

class Animation {
public:
	std::vector < Frame* > frames;
	int currentFrame;
	int currentLayer;

	Animation();
	~Animation();

	Frame* getCurrentFrame();
	std::vector < Layer* > getLayers();
	Layer* getCurrentLayer();
	std::vector < Frame* > getFrames();
	Layer* getLayer(int id);
	int getCurrentFrameID();
	int getCurrentLayerID();
	int getFramesSize();
	int getLayersSize();
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
