#pragma once
#include "Element.hpp"
#include "Textures.hpp"
#include "Controls/Slider.hpp"
#include "Controls/ButtonWithSprite.hpp"

enum class SliderWithButtonsState { Idle, Edited };

class SliderWithButtons : public Element {
public:
	SliderWithButtonsState _state;
	std::shared_ptr<Slider> _slider;
	std::shared_ptr<ButtonWithSprite> _decreaseButton;
	std::shared_ptr<ButtonWithSprite> _increaseButton;

	SliderWithButtons(std::wstring name, int min_value, int max_value, std::wstring units=L"");
	~SliderWithButtons();

	void setValue(int value);
	int getValue();
	sf::Vector2i getSize();
	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};
