#pragma once
#include "ElementGUI.hpp"
#include "Textures.hpp"

enum class SliderState { Idle, Changed };

class Slider : ElementGUI {
public:
	int _min_value;
	int _max_value;
	int _current_value;

	sf::Sprite* _bar_sprite;
	sf::Sprite* _slider_sprite;

	SliderState _state;
	sf::Vector2f _offset;

	Slider(int min_value, int max_value);
	~Slider();

	void setValue(int value);
	int getValue();
	sf::Vector2f getSliderPosition();
	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};
