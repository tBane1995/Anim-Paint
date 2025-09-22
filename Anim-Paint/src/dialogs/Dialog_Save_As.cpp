#include "Dialogs/Dialog_Save_As.hpp"

Dialog_Save_As::Dialog_Save_As() : Dialog(L"Save As", sf::Vector2f(400, 240), sf::Vector2f(8, 120)) {

	setPosition(_position);
}

Dialog_Save_As::~Dialog_Save_As() {
	
}

void Dialog_Save_As::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);
}


void Dialog_Save_As::cursorHover() {
	Dialog::cursorHover();
}

void Dialog_Save_As::handleEvent(sf::Event& event) {
	Dialog::handleEvent(event);
}

void Dialog_Save_As::update() {
	Dialog::update();
}

void Dialog_Save_As::draw() {
	Dialog::draw();
}