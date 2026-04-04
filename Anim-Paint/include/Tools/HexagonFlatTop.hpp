#pragma once
#include "SFML/Graphics.hpp"
#include "Tools/ResizableTool.hpp"



class HexagonFlatTop : public ResizableTool {
public:
	HexagonFlatTop();
	~HexagonFlatTop();

	virtual void generateImage();
};

extern std::shared_ptr<ResizableTool> resizableTool;
