#include "Components/Toolbar/OptionWithCheckbox.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"


OptionWithCheckbox::OptionWithCheckbox(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position) : Option(text, position) {
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
void OptionWithCheckbox::setPosition(sf::Vector2i position) {
	Option::setPosition(position);
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
	if (Element_hovered.get() == this) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			Element_pressed = this->shared_from_this();
			return;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (Element_pressed.get() == this) {
				click();
				return;
			}
		}
	}

	if (_isSelected && _activatedByEnter) {
		if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Enter) {
			click();
			return;
		}
	}

}

void OptionWithCheckbox::update() {
	Option::update();
	_checkbox->update();
}

void OptionWithCheckbox::draw() {
	Option::draw();
	_checkbox->draw();
}