#pragma once
#include "SFML/Graphics.hpp"
#include "Tools/ResizableTool.hpp"



class Diamond : public ResizableTool {
public:
	Diamond();
	~Diamond();

	virtual void generateImage();
};

extern std::shared_ptr<ResizableTool> resizableTool;
