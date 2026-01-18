#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include <functional>

enum class ScrollbarState { Idle, Dragging, Scrolled };

class Scrollbar : public ElementGUI {
public:

	sf::IntRect _rect;
	sf::IntRect _slider;
	
	int _min_value;
	int _max_value;
	int _slider_size;
	int _value;

	ScrollbarState _state;
	std::function<void()> _func;
	sf::Vector2i _dragOffset;

	std::shared_ptr<sf::IntRect> _scrollArea;
	int _deltaScroll;
	float _scrollStep;

	Scrollbar(int x, int y, int width, int height, int min_value, int max_value, int slider_size, int value);
	~Scrollbar();

	void setMin(int min_value);
	void setMax(int max_value);
	void setValue(int value);
	void setScrollArea(std::shared_ptr<sf::IntRect> rect, float scrollStep);

	void updateSliderSize();
	void updateSliderPosition();
	void setPosition(sf::Vector2i position);
	int getValue();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};