#include "Dialogs/Dialog.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Mouse.hpp"

Dialog::Dialog(std::wstring title, sf::Vector2f size, sf::Vector2f position) :
	_title_text(basicFont, title, dialog_title_font_size)
{

	_state = DialogState::Idle;

	_dialog_rect = sf::RectangleShape(size);
	_dialog_rect.setFillColor(dialog_border_color);

	_title_rect = sf::RectangleShape(sf::Vector2f(size.x - 2 * dialog_border_width, dialog_title_rect_height));
	_title_rect.setFillColor(dialog_title_rect_color);

	_title_text.setFillColor(dialog_title_text_color);

	_content_rect = sf::RectangleShape(sf::Vector2f(size.x - 2 * dialog_border_width, size.y - 2 * dialog_border_width - _title_rect.getSize().y));
	_content_rect.setFillColor(dialog_content_rect_color);

	_close_btn = new NormalButton(getTexture(L"tex\\dialog\\close.png"), getTexture(L"tex\\dialog\\close_hover.png"));
	_close_btn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};

	if (position == sf::Vector2f(-1, -1)) {
		sf::Vector2f pos;
		pos.x = window->getSize().x / 2 - size.x / 2;
		pos.y = window->getSize().y / 2 - size.y / 2;
		setPosition(pos);
	}
	else
		setPosition(position);

	_is_moved = false;
	_offset = sf::Vector2f(0, 0);
}

Dialog::~Dialog() {}

sf::Vector2f Dialog::getPosition() {
	return _dialog_rect.getPosition();
}

void Dialog::setSize(sf::Vector2f size) {

}

sf::Vector2f Dialog::getSize() {
	return _dialog_rect.getSize();
}

sf::Vector2f Dialog::getContentPosition() {
	return _content_rect.getPosition();
}
sf::Vector2f Dialog::getContentSize() {
	return _content_rect.getSize();
}

void Dialog::setPosition(sf::Vector2f position) {

	_position = sf::Vector2f(sf::Vector2i(position) / 8 * 8);

	_dialog_rect.setPosition(_position);
	_title_rect.setPosition(_position + sf::Vector2f(dialog_border_width, dialog_border_width));
	_close_btn->setPosition(_position + sf::Vector2f(this->getSize().x - dialog_border_width - 32, dialog_border_width));
	sf::Vector2f pos(_position + sf::Vector2f(dialog_border_width + (_title_rect.getSize().y - _title_text.getFont().getLineSpacing(dialog_title_font_size)) / 2.0f, dialog_border_width + (_title_rect.getSize().y - _title_text.getFont().getLineSpacing(dialog_title_font_size)) / 2.0f));
	_title_text.setPosition(pos);
	_content_rect.setPosition(_position + sf::Vector2f(dialog_border_width, dialog_border_width + _title_rect.getSize().y));
}

void Dialog::cursorHover() {
	if (_dialog_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	_close_btn->cursorHover();
}

void Dialog::handleEvent(const sf::Event& event) {
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (ElementGUI_hovered == this && _title_rect.getGlobalBounds().contains(worldMousePosition)) {
			_is_moved = true;
			_offset = _title_rect.getPosition() - worldMousePosition;
		}
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		_is_moved = false;
	}

	_close_btn->handleEvent(event);
}

void Dialog::update() {
	if (_is_moved) {
		setPosition(worldMousePosition + _offset);
	}

	_close_btn->update();
}

void Dialog::draw() {
	window->draw(_dialog_rect);
	window->draw(_title_rect);
	_close_btn->draw();
	window->draw(_title_text);
	window->draw(_content_rect);
}

std::vector < Dialog* > dialogs;
