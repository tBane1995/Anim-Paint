#pragma once
#include "SFML/Graphics.hpp"
#include "Tools/ResizableTool.hpp"



class Circle : public ResizableTool {
public:
	Circle();
	~Circle();

	virtual void generateImage();
};

extern std::shared_ptr<ResizableTool> resizableTool;
