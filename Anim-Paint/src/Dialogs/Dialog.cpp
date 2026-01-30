#include "Dialogs/Dialog.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Cursor.hpp"
#include "ElementGUI/TextInput.hpp"
#include "ElementGUI/NumberInput.hpp"
#include "MainMenu.hpp"
#include "Tools/Toolbar.hpp"
#include "Bottombar.hpp"
#include "Time.hpp"

Dialog::Dialog() : Dialog(L"Dialog", sf::Vector2i(128, 128)) {
	
	_absolutePositioning = true;
}

Dialog::Dialog(std::wstring title, sf::Vector2i size, sf::Vector2i position, bool absolutePositioning) : ElementGUI() {

	_title = title;

	_state = DialogState::Idle;
	
	_absolutePositioning = absolutePositioning;

	_rect  = sf::IntRect(position, size);
	_titleRect = sf::IntRect(position, sf::Vector2i(size.x - 2 * dialog_border_width, dialog_title_rect_height));

	sf::Vector2i p = position + sf::Vector2i(dialog_border_width, dialog_border_width + _titleRect.size.y);
	_contentRect = sf::IntRect(p, sf::Vector2i(size.x - 2 * dialog_border_width, size.y - 2 * dialog_border_width - _titleRect.size.y));

	_closeBtn = std::make_shared<NormalButton>(getTexture(L"tex\\btn32\\close.png"), getTexture(L"tex\\btn32\\close_hover.png"));
	_closeBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};

	_titleText = std::make_unique<sf::Text>(basicFont, _title, dialog_title_font_size);
	_titleText->setFillColor(dialog_title_text_color);

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

	_currentOnTabElement = -1;
}

Dialog::~Dialog() {

}

sf::Vector2i Dialog::getPosition() {
	return _rect.position;
}

void Dialog::setSize(sf::Vector2f size) {

}

sf::Vector2i Dialog::getSize() {
	return _rect.size;
}

sf::Vector2i Dialog::getContentPosition() {
	return _contentRect.position;
}
sf::Vector2i Dialog::getContentSize() {
	return _contentRect.size;
}

void Dialog::setPosition(sf::Vector2i position) {

	_position = position;

	_rect.position = _position;
	_titleRect.position = _position;
	_contentRect.position = _position + sf::Vector2i(dialog_border_width, dialog_border_width + _titleRect.size.y);

	sf::Vector2f pos(sf::Vector2f(_position) + sf::Vector2f(dialog_border_width + 2 + (_titleRect.size.y - _titleText->getFont().getLineSpacing(dialog_title_font_size)) / 2.0f, dialog_border_width + (_titleRect.size.y - _titleText->getFont().getLineSpacing(dialog_title_font_size)) / 2.0f));
	_titleText->setPosition(pos);

	_closeBtn->setPosition(_position + sf::Vector2i(this->getSize().x - dialog_border_width - 32, dialog_border_width));
}

void Dialog::clampPosition() {
	sf::Vector2i newPos = _position;
	newPos.x = std::clamp(newPos.x, dialog_margin, int(mainView.getSize().x) - getSize().x - dialog_margin);

	if (_absolutePositioning == true) {
		newPos.y = std::clamp(newPos.y,
			dialog_margin,
			int(mainView.getSize().y) - getSize().y - dialog_margin
		);
	}
	else {
		newPos.y = std::clamp(newPos.y,
			int(main_menu->getSize().y) + toolbar->getSize().y + dialog_margin,
			int(mainView.getSize().y) - getSize().y - bottom_bar->getSize().y - dialog_margin
		);
	}
	setPosition(newPos);
}

void Dialog::deactivateOnTabElement() {
	if (_onTabElements.size() == 0) {
		return;
	}
		
	if (_currentOnTabElement < 0) {
		return;
	}

	std::shared_ptr<NumberInput> ni = std::dynamic_pointer_cast<NumberInput>(_onTabElements[_currentOnTabElement]);

	if (ni) {
		std::wstring text = ni->getText();

		if (ni->dataIsCorrect()) {
			ni->deleteStartZeros();
			ni->_previousText = ni->_textStr;
		}
		else if (ni->isNumeric() && !text.empty() && text != L"-") {
			ni->deleteStartZeros();

			int value = std::stoi(ni->_textStr);
			value = std::clamp(value, ni->_minValue, ni->_maxValue);

			ni->_textStr = std::to_wstring(value);
		}
		else {
			ni->_textStr = ni->_previousText;
		}

		ni->_cursorPosition = (int)ni->_textStr.length();
		ni->setText(ni->_textStr);
		ni->_state = TextInputState::Idle;
		return;
	}

	

	std::shared_ptr<TextInput> ti = std::dynamic_pointer_cast<TextInput>(_onTabElements[_currentOnTabElement]);
	if (ti != nullptr) {
		ti->_state = TextInputState::Idle;
		return;
	}

	std::shared_ptr<Button> btn = std::dynamic_pointer_cast<Button>(_onTabElements[_currentOnTabElement]);
	if (btn != nullptr) {
		btn->unselect();
		return;
	}

	sf::Image img;
	const uint8_t* pixels = img.getPixelsPtr();

}


void Dialog::activateOnTabElement(int id) {

	deactivateOnTabElement();

	if (!_onTabElements.empty())
		_currentOnTabElement = id;
	else {
		_currentOnTabElement = -1;
		return;
	}
	
	if (_currentOnTabElement >= _onTabElements.size())
		_currentOnTabElement = 0;

	if (_currentOnTabElement >= 0) {
		std::shared_ptr<NumberInput> ni = std::dynamic_pointer_cast<NumberInput>(_onTabElements[_currentOnTabElement]);
		if (ni != nullptr) {
			ni->_state = TextInputState::TextEntered;
			return;
		}

		std::shared_ptr<TextInput> ti = std::dynamic_pointer_cast<TextInput>(_onTabElements[_currentOnTabElement]);
		if (ti != nullptr) {
			ti->_state = TextInputState::TextEntered;
			return;
		}

		std::shared_ptr<Button> btn = std::dynamic_pointer_cast<Button>(_onTabElements[_currentOnTabElement]);
		if (btn != nullptr) {
			btn->select();
			return;
		}
	}
}

void Dialog::cursorHover() {
	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}

	_closeBtn->cursorHover();
}

void Dialog::handleEvent(const sf::Event& event) {

	if (_rect.contains(cursor->_worldMousePosition)) {
		_clickArea = DialogClickArea::Inside;
		_startFlashTime = currentTime;
	} else {
		_clickArea = DialogClickArea::OutSide;
	 }

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (ElementGUI_hovered.get() == this && _titleRect.contains(cursor->_worldMousePosition)) {
			_is_moved = true;
			_offset = _titleRect.position - cursor->_worldMousePosition;
		}
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		_is_moved = false;
	}else if(const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Escape){
		_state = DialogState::ToClose;
	}else if (_onTabElements.size() > 0) {
		if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Tab) {

			activateOnTabElement(_currentOnTabElement + 1);
		}
	}

	_closeBtn->handleEvent(event);
}

void Dialog::update() {
	if (_is_moved) {
		setPosition(cursor->_worldMousePosition + _offset);
	}
	else {
		clampPosition();
	}

	_closeBtn->update();
}

void Dialog::draw() {

	sf::RectangleShape dialogRect(sf::Vector2f(_rect.size));
	dialogRect.setFillColor(dialog_border_color);
	dialogRect.setPosition(sf::Vector2f(_position));
	window->draw(dialogRect);

	sf::RectangleShape titleRect(sf::Vector2f(_titleRect.size));
	sf::Color titleRectColor;
	if (_absolutePositioning == true && _clickArea == DialogClickArea::OutSide) {
		int time = int(((currentTime- _startFlashTime).asSeconds()) * 2.5f) % 2;
		int c = 15 * time;
		titleRectColor = dialog_title_rect_color + sf::Color(47 * time, c, c);
	}
	else {
		titleRectColor = dialog_title_rect_color;
	}
	titleRect.setFillColor(titleRectColor);

	sf::Vector2f titleRectPosition;
	titleRectPosition.x = float(_position.x + dialog_border_width);
	titleRectPosition.y = float(_position.y + dialog_border_width);
	titleRect.setPosition(titleRectPosition);

	window->draw(titleRect);

	window->draw(*_titleText);
	_closeBtn->draw();

	sf::Vector2f contentRectSize;
	contentRectSize.x = float(_rect.size.x - 2 * dialog_border_width);
	contentRectSize.y = float(_rect.size.y - 2 * dialog_border_width - _titleRect.size.y);
	sf::RectangleShape contentRect(contentRectSize);

	contentRect.setFillColor(dialog_content_rect_color);

	sf::Vector2f contentRectPosition;
	contentRectPosition.x = float(_position.x + dialog_border_width);
	contentRectPosition.y = float(_position.y + dialog_border_width + _titleRect.size.y);
	contentRect.setPosition(contentRectPosition);

	window->draw(contentRect);
}

std::vector<std::shared_ptr<Dialog>> dialogs;
