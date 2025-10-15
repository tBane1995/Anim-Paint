#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include <functional>

enum class ScrollbarState { Idle, Dragging, Scrolled };

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

	sf::RectangleShape* _scrollArea;
	int _deltaScroll;
	float _scrollStep;

	Scrollbar(float x, float y, float width, float height, int min_value, int max_value, int slider_size, int value);
	~Scrollbar();

	void setMin(int min_value);
	void setMax(int max_value);
	void setValue(int value);
	void setScrollArea(sf::RectangleShape* rect, float scrollStep);

	void updateSliderSize();
	void updateSliderPosition();
	void setPosition(float x, float y);
	int getValue();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};