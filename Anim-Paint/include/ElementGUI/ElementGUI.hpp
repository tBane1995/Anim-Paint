#pragma once
#include "SFML/Graphics.hpp"

// std::enable_shared_from_this for this->shared_from_this
class ElementGUI : public std::enable_shared_from_this<ElementGUI> {
public:

	ElementGUI();
	virtual ~ElementGUI();

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};

extern std::shared_ptr<ElementGUI> ElementGUI_hovered;
extern std::shared_ptr<ElementGUI> ElementGUI_pressed;