#pragma once
#include "Element.hpp"
#include "Controls/Button.hpp"
#include <filesystem>
#include "Animation/Animation.hpp"

class OptionBox : public Element {
public:
	sf::RectangleShape _rect;
	std::unique_ptr<sf::Text> _text;
	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	OptionBox(std::wstring text);
	~OptionBox();

	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};