#pragma once
#include "SFML/Graphics.hpp"
#include "Tools/ResizableTool.hpp"



class Rectangle : public ResizableTool {
public:
	Rectangle();
	~Rectangle();

	virtual void generateImage();
};

extern std::shared_ptr<ResizableTool> resizableTool;
