#include "ElementGUI/NumberInput.hpp"
#include "Time.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Cursor.hpp"

NumberInput::NumberInput(sf::Vector2i size, int limitCharacters, int characterSize, int value, int minValue, int maxValue) : TextInput(size, limitCharacters, characterSize) {
	_previousText = std::to_wstring(value);
	setText(_previousText);
	_minValue = minValue;
	_maxValue = maxValue;
}

NumberInput::~NumberInput() {

}

bool NumberInput::dataIsCorrect() {
	std::wstring text = getText();
	if (text.empty()) return false;
	for (wchar_t c : text) {
		if (c < L'0' || c > L'9') {
			return false;
		}
	}

	if(std::stoi(text) < _minValue || std::stoi(text) > _maxValue)
		return false;

	return true;
}

bool NumberInput::isNumeric() {
	std::wstring text = getText();
	for (wchar_t c : text) {
		if (!(c == '-' || (c >= L'0' && c <= L'9'))) {
			return false;
		}
	}
	return true;
}

void NumberInput::deleteStartZeros() {
	// Zostaw jedno "0", jeśli cały tekst to same zera
	if (_textStr.size() == 0) return;
	if (_textStr == L"0")     return;

	while (_textStr.size() > 1 && _textStr[0] == L'0') {
		_textStr.erase(0, 1);
	}
}

int NumberInput::zerosOnStart() {
	int count = 0;
	for (wchar_t c : _textStr) {
		if (c == L'0')
			++count;
		else
			break;
	}
	return count;
}

void NumberInput::setValue(int value) {
	_textStr = std::to_wstring(value);
	_previousText = _textStr;
	_cursorPosition = _textStr.length();
	setText(_textStr);
}

int NumberInput::getValue() {
	if (dataIsCorrect()) {
		return std::stoi(_textStr);
	}
	else {
		return std::stoi(_previousText);
	}
}

void NumberInput::cursorHover() {
	TextInput::cursorHover();
}

void NumberInput::handleEvent(const sf::Event& event) {

	if (const auto* mp = event.getIf<sf::Event::MouseButtonPressed>(); mp) {

		if (_rect.contains(cursor->_worldMousePosition)) {
			_state = TextInputState::TextEntered;
			if (_onClickedFunction)
				_onClickedFunction();
		}
		else {

			if (_state == TextInputState::TextEntered) {
				if (dataIsCorrect()) {
					deleteStartZeros();
					_previousText = _textStr;
				}
				else if (!_textStr.empty() && isNumeric()) {
					_textStr = std::to_wstring(std::clamp(std::stoi(_textStr), _minValue, _maxValue));
				}
				else {
					_textStr = _previousText;
				}

				setText(_textStr);
			}

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

				if (dataIsCorrect()) {
					deleteStartZeros();
					_previousText = _textStr;
				}
				else if (!_textStr.empty() && isNumeric()) {
					_textStr = std::to_wstring(std::clamp(std::stoi(_textStr), _minValue, _maxValue));
				}
				else {
					_textStr = _previousText;
				}
				_cursorPosition = _textStr.length();
				setText(_textStr);
				if (_onEnteredFunction) {
					_onEnteredFunction();
				}
					
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
						_text->setString(_textStr);
						_cursorPosition -= 1;
						if (dataIsCorrect() && _onEditedFunction)
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
				if (_textStr.size() >= _limitCharacters)
					return;

				std::wstring c;
				c += character;
				_textStr.insert(_cursorPosition, c);
				_cursorPosition += 1;
				
				int zeros = zerosOnStart();

				if (zeros == _textStr.size()) {
					_textStr = L"0";
					_cursorPosition = 1;
				}
				else if (zeros > 0) {
					deleteStartZeros();

					_cursorPosition -= zeros;
					if (_cursorPosition < 0)
						_cursorPosition = 0;
				}

				if (_cursorPosition > _textStr.length())
					_cursorPosition = _textStr.length();
				
				_text->setString(_textStr);

				if (dataIsCorrect() && _onEditedFunction)
					_onEditedFunction();

			}
		}


	}
}

void NumberInput::update() {
	TextInput::update();
}

void NumberInput::draw() {
	// draw rect
	sf::RectangleShape rect(sf::Vector2f(_rect.size));

	sf::Color rectColor;
	switch (_state)
	{
	case TextInputState::Idle:
		rectColor = textinput_idle_color;
		break;
	case TextInputState::Hover:
		rectColor = textinput_hover_color;
		break;
	case TextInputState::TextEntered:
		rectColor = textinput_textentered_color;
		break;
	default:
		rectColor = textinput_idle_color;
		break;
	}

	rect.setFillColor((dataIsCorrect()) ? rectColor : textInput_incorrect_data_color);
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