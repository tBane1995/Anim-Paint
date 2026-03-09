#pragma once
#include "Element/Element.hpp"

class BottomBar : public Element {
public:
	sf::IntRect _rect;
	std::unique_ptr<sf::Text> _textCursorPosition;
	std::unique_ptr<sf::Text> _textCanvasSize;
	std::unique_ptr<sf::Text> _textSelectionSize;
	std::shared_ptr<Element> _hoveredElement;

	BottomBar();
	~BottomBar();

	sf::Vector2i getSize();
	void resize();
	void updateText();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};


extern std::shared_ptr<BottomBar> bottom_bar;