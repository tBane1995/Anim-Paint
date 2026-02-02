#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "SFML//Graphics.hpp"
#include <functional>

enum class TextInputState { Idle, Hover, TextEntered };

class TextInput : public ElementGUI {
public:

	sf::IntRect _rect;

	int _characterSize;
	int _limitCharacters;
	std::wstring _textStr;
	std::unique_ptr<sf::Text> _text;
	TextInputState _state;
	int _cursorPosition;
	std::function<void()> _onClickedFunction;
	std::function<void()> _onEditedFunction;
	std::function<void()> _onEnteredFunction;
	

	TextInput(sf::Vector2i size, int limitCharacters, int characterSize);
	~TextInput();
	
	void setPosition(sf::Vector2i position);
	void setText(std::wstring text);
	void setLimitCharacters(int limitCharacters);
	std::wstring getText();
	sf::Vector2i getPosition();
	sf::Vector2i getSize();
	void positioningCursorByMouse();


	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();

};