#include "Dialogs/Dialog.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Mouse.hpp"

Dialog::Dialog(std::wstring title, sf::Vector2i size, sf::Vector2i position) : ElementGUI() {

	_title = title;

	_state = DialogState::Idle;

	_dialogRect = sf::IntRect(position, size);
	_titleRect = sf::IntRect(position, sf::Vector2i(size.x - 2 * dialog_border_width, dialog_title_rect_height));

	sf::Vector2i p = position + sf::Vector2i(dialog_border_width, dialog_border_width + _titleRect.size.y);
	_contentRect = sf::IntRect(p, sf::Vector2i(size.x - 2 * dialog_border_width, size.y - 2 * dialog_border_width - _titleRect.size.y));

	_closeBtn = std::make_shared<NormalButton>(getTexture(L"tex\\dialog\\close.png"), getTexture(L"tex\\dialog\\close_hover.png"));
	_closeBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};

	if (position == sf::Vector2i(-1, -1)) {
		sf::Vector2i pos;
		pos.x = window->getSize().x / 2 - size.x / 2;
		pos.y = window->getSize().y / 2 - size.y / 2;
		setPosition(pos);
	}
	else
		setPosition(position);

	_is_moved = false;
	_offset = sf::Vector2i(0, 0);
}

Dialog::~Dialog() {

}

sf::Vector2i Dialog::getPosition() {
	return _dialogRect.position;
}

void Dialog::setSize(sf::Vector2f size) {

}

sf::Vector2i Dialog::getSize() {
	return _dialogRect.size;
}

sf::Vector2i Dialog::getContentPosition() {
	return _contentRect.position;
}
sf::Vector2i Dialog::getContentSize() {
	return _contentRect.size;
}

void Dialog::setPosition(sf::Vector2i position) {

	_position = position;
	_dialogRect.position = position;
	_titleRect.position = position;
	_contentRect.position = position + sf::Vector2i(dialog_border_width, dialog_border_width + _titleRect.size.y);

}

void Dialog::cursorHover() {
	if (_dialogRect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	_closeBtn->cursorHover();
}

void Dialog::handleEvent(const sf::Event& event) {
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (ElementGUI_hovered == this && _titleRect.contains(cursor->_worldMousePosition)) {
			_is_moved = true;
			_offset = _titleRect.position - cursor->_worldMousePosition;
		}
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		_is_moved = false;
	}

	_closeBtn->handleEvent(event);
}

void Dialog::update() {
	if (_is_moved) {
		setPosition(cursor->_worldMousePosition + _offset);
	}

	_closeBtn->update();
}

void Dialog::draw() {

	sf::RectangleShape dialogRect(sf::Vector2f(_dialogRect.size));
	dialogRect.setFillColor(dialog_border_color);
	dialogRect.setPosition(sf::Vector2f(_position));
	window->draw(dialogRect);

	sf::RectangleShape titleRect(sf::Vector2f(_titleRect.size));
	titleRect.setFillColor(dialog_title_rect_color);
	titleRect.setPosition(sf::Vector2f(_position.x + dialog_border_width, _position.y + dialog_border_width));
	window->draw(titleRect);

	_closeBtn->draw();

	if (_titleText == nullptr) {
		_titleText = std::make_unique<sf::Text>(basicFont, _title, dialog_title_font_size);
		_titleText->setFillColor(dialog_title_text_color);
	}

	sf::Vector2f pos(sf::Vector2f(_position) + sf::Vector2f(dialog_border_width + (_titleRect.size.y - _titleText->getFont().getLineSpacing(dialog_title_font_size)) / 2.0f, dialog_border_width + (_titleRect.size.y - _titleText->getFont().getLineSpacing(dialog_title_font_size)) / 2.0f));
	_titleText->setPosition(pos);
	window->draw(*_titleText);

	_closeBtn->setPosition(_position + sf::Vector2i(this->getSize().x - dialog_border_width - 32, dialog_border_width));
	_closeBtn->draw();

	sf::RectangleShape contentRect(sf::Vector2f(_dialogRect.size.x - 2 * dialog_border_width, _dialogRect.size.y - 2 * dialog_border_width - _titleRect.size.y));
	contentRect.setFillColor(dialog_content_rect_color);
	contentRect.setPosition(sf::Vector2f(_position) + sf::Vector2f(dialog_border_width, dialog_border_width + _titleRect.size.y));
	window->draw(contentRect);
}

std::vector < Dialog* > dialogs;
