#pragma once
#include "Element.hpp"
#include "Components/MainMenu/OptionBox.hpp"

class MenuBox : public Element {
public:
	sf::RectangleShape _rect;
	std::unique_ptr<sf::Text> _text;
	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;
	std::vector<std::shared_ptr<OptionBox>> _options;
	bool _isOpen;

	MenuBox(std::wstring text);
	~MenuBox();

	void addOption(std::shared_ptr<OptionBox> option);
	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};
