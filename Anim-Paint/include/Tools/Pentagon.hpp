#pragma once
#include "SFML/Graphics.hpp"
#include "Tools/ResizableTool.hpp"



class Pentagon : public ResizableTool {
public:
	Pentagon();
	~Pentagon();

	virtual void generateImage();
};

extern std::shared_ptr<ResizableTool> resizableTool;
