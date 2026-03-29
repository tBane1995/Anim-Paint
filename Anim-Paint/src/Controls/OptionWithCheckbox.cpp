#include "Controls/OptionWithCheckbox.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"


OptionWithCheckbox::OptionWithCheckbox(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, std::wstring shortcut, sf::Vector2i position) : Option(text, shortcut, position) {
	_checkbox = std::make_shared<Checkbox>(texture, hoverTexture);
	_checkbox->setValue(0);
	_checkbox->_onclick_func = [this]() {
		
	};

}

OptionWithCheckbox::~OptionWithCheckbox() {

}

void OptionWithCheckbox::addValue(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture) {
	_checkbox->addValue(texture, hoverTexture);
}
void OptionWithCheckbox::setPosition(sf::Vector2i position, int shortcut_offset) {
	Option::setPosition(position, shortcut_offset);
	_checkbox->setPosition(position);

}

void OptionWithCheckbox::click() {
	_checkbox->click();
}

void OptionWithCheckbox::cursorHover() {
	Option::cursorHover();
	_checkbox->cursorHover();
}

void OptionWithCheckbox::handleEvent(const sf::Event& event) {
	Option::handleEvent(event);
	_checkbox->handleEvent(event);
}

void OptionWithCheckbox::update() {
	Option::update();
	_checkbox->update();
}

void OptionWithCheckbox::draw() {
	Option::draw();
	_checkbox->draw();
}