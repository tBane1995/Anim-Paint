#pragma once
#include "ElementGUI/Button.hpp"

class Tooltip {
public:

	std::shared_ptr<Button> _button;
	sf::Time _timer;

	Tooltip();
	~Tooltip();

	void setButton(std::shared_ptr<Button> button);

	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

extern std::shared_ptr<Tooltip> tooltip;