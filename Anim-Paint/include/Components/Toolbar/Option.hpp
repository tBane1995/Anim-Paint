#pragma once
#include "Controls/Button.hpp"

class Option : public Button {
public:
	std::unique_ptr<sf::Text> _text;

	Option(std::wstring text, sf::Vector2i position = sf::Vector2i(0, 0));
	~Option();

	void setPosition(sf::Vector2i position);
	void draw();

};