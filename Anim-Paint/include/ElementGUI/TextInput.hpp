#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "SFML//Graphics.hpp"
#include <functional>

enum class TextInputState { Idle, TextEntered };

class TextInput : public ElementGUI {
public:

	sf::RectangleShape _rect;
	int _limitCharacters;
	sf::Text* _text;
	TextInputState _state;
	int _cursorPosition;
	sf::RectangleShape _cursor;
	std::function<void()> _onClickedFunction;
	std::function<void()> _onEditedFunction;
	std::function<void()> _onEnteredFunction;
	


	TextInput(sf::Vector2f size, int limitCharacters, int characterSize);
	~TextInput();
	
	void cursorPositioning();
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