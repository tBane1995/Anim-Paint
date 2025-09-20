#pragma once
#include "ElementGUI.hpp"
#include "Textures.hpp"

enum class SliderState { Idle, Changed };

class Slider : ElementGUI {
public:
	int min_value;
	int max_value;
	int current_value;

	Texture* bar_texture;
	Texture* slider_texture;

	sf::Sprite bar_sprite;
	sf::Sprite slider_sprite;

	SliderState state;
	sf::Vector2f offset;

	Slider(int min_value, int max_value);
	~Slider();

	void setValue(int value);
	int getValue();
	sf::Vector2f getSliderPosition();
	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};
