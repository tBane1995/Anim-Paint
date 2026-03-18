#include "Dialogs/ConfirmDialog.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Cursor.hpp"
#include "Time.hpp"


ConfirmDialog::ConfirmDialog(std::wstring title, std::wstring text, sf::Vector2i position, bool absolutePositioning) : Dialog(title, sf::Vector2i(256, 160), position, absolutePositioning) {

	_textStr = text;

	sf::Vector2i btnSize(64, basic_text_rect_height + dialog_padding);
	_confirmBtn = std::make_shared<ColoredButtonWithText>(L"Confirm", btnSize);
	_confirmBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};
	_cancelBtn = std::make_shared<ColoredButtonWithText>(L"Cancel", btnSize);
	_cancelBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};
}

ConfirmDialog::~ConfirmDialog() {

}

void ConfirmDialog::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	
	_confirmBtn->setPosition(sf::Vector2i(
		getContentPosition().x + buttonPaddingHorizontal + dialog_padding, 
		getContentPosition().y + getContentSize().y - butttonPaddingVertical - _confirmBtn->getSize().y - dialog_padding));
	_cancelBtn->setPosition(sf::Vector2i(
		getContentPosition().x - buttonPaddingHorizontal + getContentSize().x - _cancelBtn->getSize().x - dialog_padding, 
		getContentPosition().y + getContentSize().y - butttonPaddingVertical - _cancelBtn->getSize().y - dialog_padding));
}

void ConfirmDialog::cursorHover() {
	Dialog::cursorHover();

	_confirmBtn->cursorHover();
	_cancelBtn->cursorHover();
}

void ConfirmDialog::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_confirmBtn->handleEvent(event);
	_cancelBtn->handleEvent(event);
}

void ConfirmDialog::update() {
	Dialog::update();

	_confirmBtn->update();
	_cancelBtn->update();
}

void ConfirmDialog::draw() {
	Dialog::draw();

	if (_text == nullptr) {
		_text = std::make_unique<sf::Text>(basicFont, _textStr, basic_text_size);
		_text->setFillColor(basic_text_color);
	}
	sf::Vector2f filenameTextPos;
	filenameTextPos.x = (float)(getContentPosition().x + (getContentSize().x - _text->getGlobalBounds().size.x)/2);
	filenameTextPos.y = (float)(getContentPosition().y + (getContentSize().y - _text->getGlobalBounds().size.y/2 - _confirmBtn->getSize().y - 2*buttonPaddingHorizontal));
	_text->setPosition(filenameTextPos);
	window->draw(*_text);

	_confirmBtn->draw();
	_cancelBtn->draw();
}