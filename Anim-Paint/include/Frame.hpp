#pragma once
#include "Layer.hpp"

class Frame {

public:
	std::vector < Layer* > layers;

	Frame();
	~Frame();

	std::vector < Layer* >& getLayers();
};
