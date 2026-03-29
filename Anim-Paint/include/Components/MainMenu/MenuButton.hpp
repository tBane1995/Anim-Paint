#pragma once
#include "Controls/Button.hpp"
#include "Components/MainMenu/OptionBox.hpp"

class MenuButton : public Button {
public:
	std::unique_ptr<sf::Text> _text;
	std::vector<std::shared_ptr<OptionBox>> _options;

	MenuButton(std::wstring text);
	~MenuButton();

	void addOption(std::shared_ptr<OptionBox> option);
	void setPosition(sf::Vector2i position);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};
