#pragma once
#include "Controls/Button.hpp"

class ColoredButtonWithText : public Button {
public:

	std::unique_ptr < sf::Text > _text;

	ColoredButtonWithText(std::wstring text, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(0, 0));
	~ColoredButtonWithText();

	sf::Vector2i getSize();
	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};