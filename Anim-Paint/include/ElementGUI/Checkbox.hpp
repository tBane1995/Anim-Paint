#pragma once
#include <SFML/Graphics.hpp>
#include "ElementGUI.hpp"
#include "Textures.hpp"
#include <functional>

enum class CheckboxState { Idle, Hover, Pressed };

class Checkbox : public ElementGUI {
public:
	int _value;
	std::vector < Texture* > _textures;
	std::vector < Texture* > _hoverTextures;
	sf::Sprite _sprite;

	sf::Vector2f _position;
	
	CheckboxState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

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
