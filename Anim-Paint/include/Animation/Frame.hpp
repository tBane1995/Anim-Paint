#pragma once
#include "Layer.hpp"

class Frame {

public:
	std::vector < Layer* > _layers;

	Frame();
	~Frame();

	std::vector < Layer* >& getLayers();
	Layer* getLayer(int id);
};
