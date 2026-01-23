#pragma once
#include "ElementGUI/ElementGUI.hpp"

class BottomBar : public ElementGUI {
public:
	sf::IntRect _rect;
	std::unique_ptr<sf::Text> _textCursorPosition;
	std::unique_ptr<sf::Text> _textCanvasSize;
	std::unique_ptr<sf::Text> _textSelectionSize;
	std::shared_ptr<ElementGUI> _hoveredElementGUI;

	BottomBar();
	~BottomBar();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};


extern std::shared_ptr<BottomBar> bottom_bar;