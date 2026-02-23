#pragma once
#include "ElementGUI/Button.hpp"

class Tooltip {
public:

	std::shared_ptr<Button> _button;
	sf::Time _timer;

	std::wstring _title;
	std::wstring _description;

	std::shared_ptr<sf::Text> _title_text;
	std::shared_ptr<sf::Text> _description_text;

	Tooltip();
	~Tooltip();

	void setButton(std::shared_ptr<Button> button);
	float getHeightOfTitleAndDescription();
	void update();
	void draw();
};

extern std::shared_ptr<Tooltip> tooltip;