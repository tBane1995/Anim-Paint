#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "SFML//Graphics.hpp"

class TextInput : public ElementGUI {
public:

	sf::RectangleShape _rect;
	int _limitCharacters;
	sf::Text _text;
	sf::RectangleShape _cursor;

	TextInput(sf::Vector2f size, int limitCharacters, int characterSize);
	~TextInput();
	
	void setPosition(sf::Vector2f position);
	void setText(std::wstring text);
	void setLimitCharacters(int limitCharacters);
	std::wstring getText();
	sf::Vector2f getPosition();
	sf::Vector2f getSize();


	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};