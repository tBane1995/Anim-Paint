#pragma once
#include "Element.hpp"
#include "Controls/Button.hpp"
#include <filesystem>
#include "Animation/Animation.hpp"

class OptionBox : public Button {
public:
	std::unique_ptr<sf::Text> _text;
	std::unique_ptr<sf::Text> _shortcut_text;

	OptionBox(std::wstring text, std::wstring shortcut = L"");
	~OptionBox();

	void setPosition(sf::Vector2i position, sf::Vector2i size);
	void draw();
};