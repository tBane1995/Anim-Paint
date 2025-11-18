#pragma once
#include "Layer.hpp"

class Frame {

public:
	std::vector<std::shared_ptr<Layer>> _layers;

	Frame();
	~Frame();

	std::vector<std::shared_ptr<Layer>>& getLayers();
	std::shared_ptr<Layer> getLayer(int id);
	int getLayersCount();
	void addEmptyLayer();
	void addLayer(std::shared_ptr<Layer> layer);
};
