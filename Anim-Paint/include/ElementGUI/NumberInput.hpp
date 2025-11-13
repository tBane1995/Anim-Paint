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
	void deleteStartZeros();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
	
};