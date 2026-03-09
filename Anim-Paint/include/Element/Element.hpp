#pragma once
#include "SFML/Graphics.hpp"

// std::enable_shared_from_this for this->shared_from_this
class Element : public std::enable_shared_from_this<Element> {
public:

	Element();
	virtual ~Element();

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};

extern std::shared_ptr<Element> Element_hovered;
extern std::shared_ptr<Element> Element_pressed;