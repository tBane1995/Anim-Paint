#include "ElementGUI/TextInput.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Time.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include "Cursor.hpp"
#include "DebugLog.hpp"

TextInput::TextInput(sf::Vector2i size, int limitCharacters, int characterSize) : ElementGUI() {
	
	_rect = sf::IntRect(sf::Vector2i(0,0), size);
	
	_limitCharacters = limitCharacters;
	_characterSize = characterSize;
	_textStr = L"";

	_text = std::make_unique<sf::Text>(basicFont, L"", _characterSize);
	_text->setFillColor(sf::Color(191, 191, 191));
	_text->setString(_textStr);

	_state = TextInputState::Idle;
	_lastCLickTime = sf::Time::Zero;
	_editState = TextInputEditState::None;

	_cursorPosition = 0;
	_selectionStart = -1;
	_selectionEnd = -1;

	_onEditedFunction = { };
	_onClickedFunction = { };
	_onEnteredFunction = { };

}

TextInput::~TextInput() {
	
}

void TextInput::setPosition(sf::Vector2i position) {
	_rect.position = position;

	sf::Vector2f textPosition;
	textPosition.x = (float)_rect.position.x + (float)(textInput_border_width);
	textPosition.y = (float)_rect.position.y + (float)(textInput_border_width);
	textPosition.y += (float)(_rect.size.y-2*textInput_border_width - basicFont.getLineSpacing(_text->getCharacterSize())) / 2.f;
	_text->setPosition(textPosition);
}

void TextInput::setText(std::wstring text) {
	_textStr = text.substr(0, _limitCharacters);
	_cursorPosition = (int)_textStr.length();
	_text->setString(_textStr.substr(0, _limitCharacters));
}

void TextInput::setLimitCharacters(int limitCharacters) {
	_limitCharacters = limitCharacters;
	_textStr = _textStr.substr(0,_limitCharacters);

	if(_cursorPosition > limitCharacters)
		_cursorPosition = _limitCharacters;

	_text->setString(_textStr.substr(0, _limitCharacters));
}

std::wstring TextInput::getText() {
	return _textStr;
}

sf::Vector2i TextInput::getPosition() {
	return _rect.position;
}

sf::Vector2i TextInput::getSize() {
	return _rect.size;
}

void TextInput::positioningCursorByMouse() {

	int newCursorPosition = 0;

	for (int i = 0; i < (int)(_textStr.length()); i++) {
		sf::Vector2f charPos = _text->findCharacterPos(i);
		if (cursor->_worldMousePosition.x > charPos.x) {
			newCursorPosition = i + 1;
		}
	}

	_cursorPosition = newCursorPosition;
}

void TextInput::cursorHover() {
	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
		return;
	}

	if (_editState == TextInputEditState::Selecting) {
		ElementGUI_hovered = this->shared_from_this();
		return;
	}

}

void TextInput::handleEvent(const sf::Event& event) {
	
	if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>(); mp) {
		if (_rect.contains(cursor->_worldMousePosition)) {

			if (_editState == TextInputEditState::TextEntered) {
				if ((currentTime - _lastCLickTime).asSeconds() < 0.2f) {
					// double click
					_selectionStart = 0;
					_selectionEnd = (int)_textStr.length();
					_cursorPosition = _selectionEnd;
					_editState = TextInputEditState::Selected;
				}
				else {
					positioningCursorByMouse();
					_selectionStart = _cursorPosition;
					_selectionEnd = _cursorPosition;

				}
			}
			else {

				if ((currentTime - _lastCLickTime).asSeconds() < 0.2f) {
					// double click
					_selectionStart = 0;
					_selectionEnd = (int)_textStr.length();
					_cursorPosition = _selectionEnd;
					_editState = TextInputEditState::Selected;
				}
				else {
					positioningCursorByMouse();
					_selectionStart = _cursorPosition;
					_selectionEnd = _cursorPosition;
					
					_editState = TextInputEditState::TextEntered;
					if (_onClickedFunction)
						_onClickedFunction();
				}
				
			}

			ElementGUI_pressed = this->shared_from_this();
		}
		else {
			_editState = TextInputEditState::None;
			_selectionStart = -1;
			_selectionEnd = -1;
			_lastCLickTime = currentTime;

			if(ElementGUI_pressed.get() == this)
				ElementGUI_pressed = nullptr;
		}
		_lastCLickTime = currentTime;
		return;
	}

	if(const auto* mr = event.getIf<sf::Event::MouseButtonReleased>(); mr) {
		if (ElementGUI_pressed.get() == this) {
			ElementGUI_pressed = nullptr;
		}

		if(_editState == TextInputEditState::Selecting)
			_editState = TextInputEditState::Selected;

		return;
	}

	if (_editState == TextInputEditState::Selecting || _editState == TextInputEditState::TextEntered) {
		if (const auto* mm = event.getIf<sf::Event::MouseMoved>(); mm && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

			positioningCursorByMouse();
			_selectionEnd = _cursorPosition;

			if (_editState == TextInputEditState::TextEntered) {
				if (!(_selectionStart == -1 && _selectionEnd == -1) && _selectionEnd != _selectionStart) {
					_editState = TextInputEditState::Selecting;
					
				}
			}

			return;
		}
	}

	if (_editState == TextInputEditState::TextEntered || _editState == TextInputEditState::Selected) {
		if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp) {

			if (kp->code == sf::Keyboard::Key::Left) {
				if (_cursorPosition > 0) {
					_cursorPosition -= 1;
				}
			}
			else if (kp->code == sf::Keyboard::Key::Right) {
				if (_cursorPosition < _text->getString().getSize()) {
					_cursorPosition += 1;
				}
			}
			else if (kp->code == sf::Keyboard::Key::Enter) {
				if (_onEnteredFunction)
					_onEnteredFunction();
			}
			return;
		}
		else if (const auto* te = event.getIf<sf::Event::TextEntered>(); te) {

				wchar_t character = (wchar_t)te->unicode;

				if (character == 8) {
					// BACKSPACE
					if (!_textStr.empty()) {
						if (_cursorPosition > 0) {
							_textStr.erase(_cursorPosition - 1, 1);
							_text->setString(_textStr.substr(0, _limitCharacters));
							_cursorPosition -= 1;
							if (_onEditedFunction)
								_onEditedFunction();
						}
					}
					return;
				}
				else if (character == 13 || character == 10) {
					// ENTER
					return;
				}
				else if (character >= 32) {
					std::wstring c;
					c += character;
					_textStr.insert(_cursorPosition, c);
					_text->setString(_textStr.substr(0, _limitCharacters));
					_cursorPosition += 1;
					if (_onEditedFunction)
						_onEditedFunction();

				}
			}
			

	}
	
}

void TextInput::update() {

	if (_editState == TextInputEditState::TextEntered) {
	
	}
	
	if (ElementGUI_hovered.get() == this) {
		_state = TextInputState::Hover;
	}
	else {
		_state = TextInputState::Idle;
	}
}

void TextInput::draw() {
	// draw rect

	sf::Vector2f rectSize;
	rectSize.x = (float)(_rect.size.x - 2 * textInput_border_width);
	rectSize.y = (float)(_rect.size.y - 2 * textInput_border_width);
	sf::RectangleShape rect(rectSize);

	if (_editState == TextInputEditState::TextEntered) {
		rect.setFillColor(textinput_textentered_color);
	}
	else if(_state == TextInputState::Hover) {
		rect.setFillColor(textinput_hover_color);
	}
	else {
		rect.setFillColor(textinput_idle_color);
	}
	
	rect.setOutlineThickness((float)textInput_border_width);
	rect.setOutlineColor(textInput_border_color);

	sf::Vector2f rectPosition;
	rectPosition.x = (float)(_rect.position.x + textInput_border_width);
	rectPosition.y = (float)(_rect.position.y + textInput_border_width);
	rect.setPosition(rectPosition);

	window->draw(rect);

	if (!(_selectionStart == -1 && _selectionEnd == -1) && _selectionStart != _selectionEnd) {

		int selection_margin = 1;

		int min = std::min(_selectionStart, _selectionEnd);
		int max = std::max(_selectionStart, _selectionEnd);

		sf::Vector2f selectionRectSize;
		selectionRectSize.x = _text->findCharacterPos(max).x - _text->findCharacterPos(min).x;
		selectionRectSize.y = _rect.size.y - 2 * textInput_border_width - 2* selection_margin;

		sf::Vector2f selectionRectPosition;
		selectionRectPosition.x = _text->findCharacterPos(min).x;
		selectionRectPosition.y = (float)_rect.position.y + (float)textInput_border_width + selection_margin;
		
		sf::RectangleShape selectionRect(selectionRectSize);

		selectionRect.setPosition(selectionRectPosition);
		selectionRect.setFillColor(sf::Color(31, 31, 127, 255));

		window->draw(selectionRect);

	}

	// draw text
	window->draw(*_text);

	
	// draw cursor
	if (_editState == TextInputEditState::TextEntered && int(currentTime.asSeconds() * 3) % 2 == 0) {
		sf::RectangleShape cursor(sf::Vector2f(2, basicFont.getLineSpacing(_characterSize)));
		cursor.setFillColor(sf::Color::Red);
		cursor.setPosition(_text->findCharacterPos(_cursorPosition));
		window->draw(cursor);
	}
		

}


