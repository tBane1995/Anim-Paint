#include "ElementGUI/TextInput.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Time.hpp"
#include "SFML/Graphics.hpp"
#include <iostream>
#include "Mouse.hpp"

TextInput::TextInput(sf::Vector2f size, int limitCharacters, int characterSize) :
	ElementGUI(),
	_text(basicFont, L"", characterSize)
{
	_rect = sf::RectangleShape(size);
	_rect.setFillColor(sf::Color(79, 79, 79));

	_limitCharacters = limitCharacters;

	_state = TextInputState::Idle;

	_text.setFillColor(sf::Color(191, 191, 191));

	_cursorPosition = 0;

	_cursor = sf::RectangleShape(sf::Vector2f(2, basicFont.getLineSpacing(characterSize)));
	_cursor.setFillColor(sf::Color::Red);
	_cursor.setPosition(sf::Vector2f(_text.getGlobalBounds().size.x, (_rect.getSize().y - _cursor.getSize().y) / 2.f));

	_function = { };

	setPosition(sf::Vector2f(0, 0));
}

TextInput::~TextInput() {

}

void TextInput::cursorPositioning() {
	_cursor.setPosition(_text.findCharacterPos(_cursorPosition));
}

void TextInput::setPosition(sf::Vector2f position) {
	_rect.setPosition(position);	
	_text.setPosition(position + sf::Vector2f(0, (_rect.getSize().y - basicFont.getLineSpacing(_text.getCharacterSize())) / 2.f));
	cursorPositioning();
}

void TextInput::setText(std::wstring text) {

	_text.setString(text.substr(0,_limitCharacters));
	_cursor.setPosition(sf::Vector2f(_text.getPosition().x + _text.getGlobalBounds().size.x, _rect.getPosition().y + (_rect.getSize().y - _cursor.getSize().y) / 2.f));
	_cursorPosition = text.length();
}

void TextInput::setLimitCharacters(int limitCharacters) {
	_limitCharacters = limitCharacters;
	_text.setString(_text.getString().substring(0,_limitCharacters));
	_cursorPosition = _limitCharacters;
}

std::wstring TextInput::getText() {
	return _text.getString();
}

sf::Vector2f TextInput::getPosition() {
	return _rect.getPosition();
}

sf::Vector2f TextInput::getSize() {
	return _rect.getSize();
}
void TextInput::cursorHover() {

}

void TextInput::handleEvent(const sf::Event& event) {
	

	if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>(); mp) {
		
		if (_rect.getGlobalBounds().contains(worldMousePosition)) {
			_state = TextInputState::TextEntered;
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
					cursorPositioning();
				}
			}
			else if (kp->code == sf::Keyboard::Key::Right) {
				if (_cursorPosition < _text.getString().getSize()) {
					_cursorPosition += 1;
					cursorPositioning();
				}
			}
			else if (kp->code == sf::Keyboard::Key::Enter) {
				if (_function)
					_function();
			}
			return;
		}
		else {
			if (const auto* te = event.getIf<sf::Event::TextEntered>(); te) {

				char32_t character = te->unicode;

				if (character == 8) {
					// BACKSPACE
					sf::String s = _text.getString();
					if (!s.isEmpty()) {
						s.erase(s.getSize() - 1, 1);
						setText(s.toWideString());
					}
					return;
				}
				else if (character == 13 || character == 10) {
					// ENTER
					return;
				}
				else if (character >= 32) {
					sf::String s = _text.getString();
					s.insert(_cursorPosition, character);
					_text.setString(s);
					_cursorPosition += 1;
					cursorPositioning();


				}
			}
		}
	}
	
}

void TextInput::update() {

}

void TextInput::draw() {

	window->draw(_rect);
	window->draw(_text);
	if(_state == TextInputState::TextEntered && int(currentTime.asSeconds()*3)%2 == 0)
		window->draw(_cursor);

}


