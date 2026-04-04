#pragma once
#include "SFML/Graphics.hpp"
#include "Tools/ResizableTool.hpp"



class HexagonPointTop : public ResizableTool {
public:
	HexagonPointTop();
	~HexagonPointTop();

	virtual void generateImage();
};

extern std::shared_ptr<ResizableTool> resizableTool;
