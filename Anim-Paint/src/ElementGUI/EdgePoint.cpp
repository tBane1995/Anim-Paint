#include "ElementGUI/EdgePoint.hpp"
#include "Cursor.hpp"

EdgePoint::EdgePoint(sf::Vector2i position) {
	_rect = sf::IntRect(position, sf::Vector2i(8, 8));
}

EdgePoint::~EdgePoint() {

}

void EdgePoint::setPosition(sf::Vector2i position){
	_rect.position = position;
}

sf::Vector2i EdgePoint::getPosition() {
	return _rect.position;
}

sf::Vector2i EdgePoint::getSize() {
	return _rect.size;
}
void EdgePoint::cursorHover() {

	sf::IntRect r(_rect.position - _rect.size / 2, _rect.size);
	if (r.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}
}

void EdgePoint::handleEvent(const sf::Event& event) {
	if (ElementGUI_pressed.get() == this) {
		if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = nullptr;
			return;
		}
	}

	sf::IntRect r(_rect.position - _rect.size / 2, _rect.size);
	if (r.contains(cursor->_worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
	}
}

void EdgePoint::update() {
	
}

void EdgePoint::draw() {
	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setOrigin(sf::Vector2f(_rect.size/2));
	rect.setFillColor(sf::Color(128, 47, 47, 255));
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);
}
