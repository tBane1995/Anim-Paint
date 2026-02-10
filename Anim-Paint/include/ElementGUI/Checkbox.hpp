#pragma once
#include <SFML/Graphics.hpp>
#include "ElementGUI.hpp"
#include "Textures.hpp"
#include <functional>

enum class CheckboxState { Idle, Hover, Pressed };

class Checkbox : public ElementGUI {
public:
	int _value;
	sf::IntRect _rect;
	std::vector<std::shared_ptr<Texture>> _textures;
	std::vector<std::shared_ptr<Texture>> _hoverTextures;
	
	CheckboxState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	Checkbox(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture);
	~Checkbox();

	sf::Vector2i getSize();
	void setPosition(sf::Vector2i position);
	void setValue(int value);
	void addValue(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
