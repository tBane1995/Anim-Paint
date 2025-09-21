#pragma once
#include <SFML/Graphics.hpp>
#include "ElementGUI.hpp"
#include "Textures.hpp"
#include <functional>

enum class CheckboxState { Idle, Hover, Pressed };

class Checkbox : public ElementGUI {
public:
	int value;
	std::vector < Texture* > textures;
	std::vector < Texture* > hoverTextures;
	sf::Sprite sprite;

	sf::Vector2f position;
	
	CheckboxState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;

	Checkbox();
	~Checkbox();

	void setPosition(sf::Vector2f position);
	void setValue(int value);
	void addValue(Texture* texture, Texture* hoverTexture);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};
