#pragma once
#include "ElementGUI.hpp"
#include "Textures.hpp"

enum class SliderState { Idle, Hovered, Pressed };
enum class SliderEditState { None, Changed };

class Slider : public ElementGUI {
public:
	std::wstring _name;
	std::wstring _units;
	int _min_value;
	int _max_value;
	int _current_value;

	sf::IntRect _barRect;
	sf::IntRect _thumbRect;

	SliderState _state;
	SliderEditState _editState;

	std::unique_ptr<sf::Text> _nameText;
	std::unique_ptr<sf::Text> _valueText;

	Slider(std::wstring name, int min_value, int max_value, std::wstring units=L"");
	~Slider();

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
