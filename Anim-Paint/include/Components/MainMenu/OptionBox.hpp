#pragma once
#include "Element.hpp"
#include "Controls/Button.hpp"
#include <filesystem>
#include "Animation/Animation.hpp"

class OptionBox : public Button {
public:
	std::unique_ptr<sf::Text> _text;

	OptionBox(std::wstring text);
	~OptionBox();

	void setPosition(sf::Vector2i position);
	void setSize(sf::Vector2i size);
	void draw();
};