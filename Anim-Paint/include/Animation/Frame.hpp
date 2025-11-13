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
};
