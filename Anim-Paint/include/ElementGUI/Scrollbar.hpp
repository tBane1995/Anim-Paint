#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include <functional>

enum class ScrollbarState { Idle, Dragging };

class Scrollbar : ElementGUI {
public:

	sf::RectangleShape _rect;
	sf::RectangleShape _slider;

	int _min_value;
	int _max_value;
	int _slider_size;
	int _value;

	ScrollbarState _state;
	std::function<void()> _func;
	sf::Vector2f _dragOffset;

	Scrollbar(float x, float y, float width, float height, int min_value, int max_value, int slider_size, int value);
	~Scrollbar();

	void updateSliderSize();
	void updateSliderPosition();
	void setPosition(float x, float y);
	int getValue();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};