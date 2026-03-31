#include "Controls/SliderWithButtons.hpp"
#include "Element.hpp"
#include "Textures.hpp"
#include "SFML/Graphics.hpp"
#include "Cursor.hpp"
#include "Window.hpp"
#include "Theme.hpp"

SliderWithButtons::SliderWithButtons(std::wstring name, int min_value, int max_value, std::wstring units) : Element() {
	
	_slider = std::make_shared<Slider>(name, min_value, max_value, units);
	_decreaseButton = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\dialog\\decrease.png"), getTexture(L"tex\\dialog\\decrease_hover.png"), getTexture(L"tex\\dialog\\decrease_hover.png"));
	_increaseButton = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\dialog\\increase.png"), getTexture(L"tex\\dialog\\increase_hover.png"), getTexture(L"tex\\dialog\\increase_hover.png"));
	
	_decreaseButton->_onclick_func = [this]() {
		_slider->setValue(_slider->getValue() - 1);
		};

	_increaseButton->_onclick_func = [this]() {
		_slider->setValue(_slider->getValue() + 1);
		};

	
}

SliderWithButtons::~SliderWithButtons() {

}

void SliderWithButtons::setValue(int value) {
	_slider->setValue(value);
}

int SliderWithButtons::getValue() {
	return _slider->getValue();
}

sf::Vector2i SliderWithButtons::getSize() {
	sf::Vector2i size;
	size.x = _slider->getSize().x + 2 + _increaseButton->getSize().x + 2 + _decreaseButton->getSize().x;
	size.y = _slider->getSize().y;
	return size;
}

void SliderWithButtons::setPosition(sf::Vector2i position) {
	_slider->setPosition(position);
	_decreaseButton->setPosition(position + sf::Vector2i(2 + _slider->getSize().x, 0));
	_increaseButton->setPosition(position + sf::Vector2i(2 + _slider->getSize().x + 2 +_decreaseButton->getSize().x, 0));

}

void SliderWithButtons::cursorHover() {
	_slider->cursorHover();
	_decreaseButton->cursorHover();
	_increaseButton->cursorHover();
}

void SliderWithButtons::handleEvent(const sf::Event& event) {

	_slider->handleEvent(event);
	_decreaseButton->handleEvent(event);
	_increaseButton->handleEvent(event);
}

void SliderWithButtons::update() {
	_slider->update();
	_decreaseButton->update();
	_increaseButton->update();
}


void SliderWithButtons::draw() {
	_slider->draw();
	_decreaseButton->draw();
	_increaseButton->draw();
}