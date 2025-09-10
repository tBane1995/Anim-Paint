#ifndef Animation_hpp
#define Animation_hpp

class Animation {
public:
	std::vector < Frame* > frames;
	int currentFrame;
	int currentLayer;

	Animation() {

		currentFrame = 0;
		currentLayer = 0;

		frames.clear();
		frames.push_back(new Frame());
		frames.push_back(new Frame());
		
		lastLayer();
		
	}

	~Animation() {

	}

	Frame* getCurrentFrame() {
		// return current Frame
		if (frames.empty())
			return nullptr;

		return frames[currentFrame];
	}

	std::vector < Layer* > getLayers() {
		// return current Layers
		if (frames.empty())
			return std::vector < Layer* >();

		return getCurrentFrame()->getLayers();
	}

	Layer* getCurrentLayer() {
		// return current Layer
		if (frames.empty())
			return nullptr;

		return getCurrentFrame()->getLayers()[currentLayer];
	}

	std::vector < Frame* > getFrames() {
		return frames;
	}

	Layer* getLayer(int id) {
		// return Layer
		if (id < 0) 
			return nullptr;

		else if (id >= getCurrentFrame()->getLayers().size())
			return nullptr;

		else 
			return getCurrentFrame()->getLayers()[id];
	}

	int getCurrentFrameID() {
		// return current Frame ID
		return currentFrame;
	}

	int getCurrentLayerID() {
		// return current Layer ID
		return currentLayer;
	}

	int getFramesSize() {
		// return Frames size
		return frames.size();
	}

	int getLayersSize() {
		// return current Layers size
		return getCurrentFrame()->layers.size();
	}

	void setCurrentFrameID(int id) {
		// set current Frame ID
		currentFrame = id;
	}

	void setCurrentLayerID(int id) {
		// set current Layer ID
		currentLayer = id;
	}

	void firstFrame() {
		currentFrame = 0;
	}

	void lastFrame() {
		currentFrame = frames.size() - 1;
	}

	void nextFrame() {
		currentFrame += 1;

		if (currentFrame >= frames.size())
			currentFrame = 0;
	}

	void prevFrame() {
		currentFrame -= 1;

		if (currentFrame < 0)
			currentFrame = frames.size() - 1;
	}

	void firstLayer() {
		currentLayer = 0;
	}

	void lastLayer() {
		currentLayer = getCurrentFrame()->layers.size() - 1;
	}
};


Animation* animation = nullptr;

#endif