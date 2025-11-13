#pragma once
#include "ElementGUI.hpp"
#include "Textures.hpp"

enum class SliderState { Idle, Changed };

class Slider : public ElementGUI {
public:
	int _min_value;
	int _max_value;
	int _current_value;

	sf::IntRect _barRect;
	sf::IntRect _sliderRect;

	SliderState _state;
	sf::Vector2i _offset;

	Slider(int min_value, int max_value);
	~Slider();

	void setValue(int value);
	int getValue();
	sf::Vector2i getSliderPosition();
	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};
