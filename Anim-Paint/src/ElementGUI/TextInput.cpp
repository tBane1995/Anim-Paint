#include "ElementGUI/TextInput.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Time.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include "Cursor.hpp"

TextInput::TextInput(sf::Vector2i size, int limitCharacters, int characterSize) : ElementGUI() {
	_rect = sf::IntRect(sf::Vector2i(0,0), size);
	
	_limitCharacters = limitCharacters;
	_characterSize = characterSize;
	_textStr = L"";

	_text = std::make_unique<sf::Text>(basicFont, L"", _characterSize);
	_text->setFillColor(sf::Color(191, 191, 191));
	_text->setString(_textStr);

	_state = TextInputState::Idle;

	_cursorPosition = 0;

	_onEditedFunction = { };
	_onClickedFunction = { };
	_onEnteredFunction = { };

}

TextInput::~TextInput() {
	
}

void TextInput::setPosition(sf::Vector2i position) {
	_rect.position = position;
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(0, (_rect.size.y - basicFont.getLineSpacing(_text->getCharacterSize())) / 2.f));
}

void TextInput::setText(std::wstring text) {
	_textStr = text.substr(0, _limitCharacters);
	_cursorPosition = _textStr.length();
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
void TextInput::cursorHover() {
	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}
}

void TextInput::handleEvent(const sf::Event& event) {
	

	if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>(); mp) {
		
		if (_rect.contains(cursor->_worldMousePosition)) {
			_state = TextInputState::TextEntered;
			if (_onClickedFunction)
				_onClickedFunction();
		}
		else {
			_state = TextInputState::Idle;
		}

		return;
	}

	if (_state == TextInputState::TextEntered) {
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

				char32_t character = te->unicode;

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

	if (_state == TextInputState::TextEntered) {

	}
	else if (ElementGUI_hovered.get() == this) {
		_state = TextInputState::Hover;
	}
	else {
		_state = TextInputState::Idle;
	}
}

void TextInput::draw() {
	// draw rect
	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	switch (_state)
	{
	case TextInputState::Idle:
		rect.setFillColor(textinput_idle_color);
		break;
	case TextInputState::Hover:
		rect.setFillColor(textinput_hover_color);
		break;
	case TextInputState::TextEntered:
		rect.setFillColor(textinput_textentered_color);
		break;
	default:
		rect.setFillColor(textinput_idle_color);
		break;
	}
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);

	// draw text
	window->draw(*_text);
	
	// draw cursor
	if (_state == TextInputState::TextEntered && int(currentTime.asSeconds() * 3) % 2 == 0) {
		sf::RectangleShape cursor(sf::Vector2f(2, basicFont.getLineSpacing(_characterSize)));
		cursor.setFillColor(sf::Color::Red);
		cursor.setPosition(_text->findCharacterPos(_cursorPosition));;
		window->draw(cursor);
	}
		

}


