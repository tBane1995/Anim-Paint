#pragma once
#include "ElementGUI.hpp"
#include "Textures.hpp"

enum class BigSliderState { Idle, Hovered, Pressed };
enum class BigSliderEditState { None, Changed };

class BigSlider : public ElementGUI {
public:
	std::wstring _name;
	int _min_value;
	int _max_value;
	int _current_value;

	sf::IntRect _barRect;
	sf::IntRect _thumbRect;

	BigSliderState _state;
	BigSliderEditState _editState;

	std::unique_ptr<sf::Text> _nameText;
	std::unique_ptr<sf::Text> _valueText;

	BigSlider(std::wstring name, int min_value, int max_value);
	~BigSlider();

	void setValue(int value);
	int getValue();
	sf::Vector2i getSize();
	int getThumbWidth();
	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};
