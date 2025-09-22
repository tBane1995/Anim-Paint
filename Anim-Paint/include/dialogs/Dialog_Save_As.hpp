#pragma once
#include "Dialog.hpp"


class Dialog_Save_As : public Dialog {
public:

	Dialog_Save_As();
	~Dialog_Save_As();

	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};