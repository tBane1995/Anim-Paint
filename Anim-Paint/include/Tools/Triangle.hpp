#pragma once
#include "SFML/Graphics.hpp"
#include "Tools/ResizableTool.hpp"



class Triangle : public ResizableTool {
public:
	Triangle();
	~Triangle();

	virtual void generateImage();
};

extern std::shared_ptr<ResizableTool> resizableTool;
