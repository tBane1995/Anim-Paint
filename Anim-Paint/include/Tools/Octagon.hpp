#pragma once
#include "SFML/Graphics.hpp"
#include "Tools/ResizableTool.hpp"



class Octagon : public ResizableTool {
public:
	Octagon();
	~Octagon();

	virtual void generateImage();
};

extern std::shared_ptr<ResizableTool> resizableTool;
