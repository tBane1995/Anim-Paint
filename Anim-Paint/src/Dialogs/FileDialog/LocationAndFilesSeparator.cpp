#include "Dialogs/FileDialog/LocationAndFilesSeparator.hpp"
#include "Theme.hpp"
#include "Cursor.hpp"

LocationAndFilesSeparator::LocationAndFilesSeparator(int linesCount) : Element() {
	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(file_dialog_separator_width, linesCount * basic_text_rect_height));
	_minX = _maxX = 0;
	_state = LocationAndFilesSeparatorState::Idle;
}

LocationAndFilesSeparator::~LocationAndFilesSeparator() {

}

sf::Vector2i LocationAndFilesSeparator::getSize() {
	return _rect.size;
}

void LocationAndFilesSeparator::setPosition(sf::Vector2i position) {
	_rect.position = position;
}

sf::Vector2i LocationAndFilesSeparator::getPosition() {
	return _rect.position;
}

void LocationAndFilesSeparator::setRange(int minX, int maxX) {
	_minX = minX;
	_maxX = maxX;
}

void LocationAndFilesSeparator::cursorHover() {
	if (_rect.contains(cursor->_position) || _state == LocationAndFilesSeparatorState::Moving) {
		Element_hovered = this->shared_from_this();
	}
}

void LocationAndFilesSeparator::handleEvent(const sf::Event& event) {
	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (Element_pressed.get() == this) {
			Element_pressed = nullptr;
			_state = LocationAndFilesSeparatorState::Idle;
		}
	}
	else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_rect.contains(cursor->_position)) {
			Element_pressed = this->shared_from_this();
			_state = LocationAndFilesSeparatorState::Moving;
			_offset = cursor->_position - _rect.position;

		}
	}
}

void LocationAndFilesSeparator::update() {

	if (_state == LocationAndFilesSeparatorState::Moving) {

		sf::Vector2i newPos = cursor->_position - _offset;
		newPos.x = std::clamp(newPos.x, _minX, _maxX);
		_rect.position = sf::Vector2i(newPos.x, _rect.position.y);
		if (_func)
			_func();
	}
}

void LocationAndFilesSeparator::draw() {

	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setPosition(sf::Vector2f(_rect.position));
	switch (_state)
	{
	case LocationAndFilesSeparatorState::Idle:
		rect.setFillColor(file_dialog_separator_idle_color);
		break;
	case LocationAndFilesSeparatorState::Hover:
		rect.setFillColor(file_dialog_separator_hover_color);
		break;
	case LocationAndFilesSeparatorState::Moving:
		rect.setFillColor(file_dialog_separator_press_color);
		break;
	}
	window->draw(rect);
}