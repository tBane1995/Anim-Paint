#pragma once
#include "ElementGUI/TextInput.hpp"

class NumberInput : public TextInput {
public:

	std::wstring _previousText;
	int _minValue;
	int _maxValue;

	NumberInput(sf::Vector2i size, int limitCharacters, int characterSize, int value, int minValue, int maxValue);
	~NumberInput();

	bool dataIsCorrect();
	bool isNumeric();
	void deleteStartZeros();
	int zerosOnStart();
	void setValue(int value);
	int getValue();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
	
};