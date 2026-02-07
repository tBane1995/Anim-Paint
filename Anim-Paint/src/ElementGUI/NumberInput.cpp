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
	_cursorPosition = (int)_textStr.length();
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

			if (ElementGUI_pressed.get() == this)
				ElementGUI_pressed = nullptr;
		}
		_lastCLickTime = currentTime;
		return;
	}

	if (const auto* mr = event.getIf<sf::Event::MouseButtonReleased>(); mr) {
		if (ElementGUI_pressed.get() == this) {
			ElementGUI_pressed = nullptr;
		}

		if (_editState == TextInputEditState::Selecting)
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

				_cursorPosition = (int)_textStr.length();
				setText(_textStr);
				if (_onEnteredFunction) {
					_onEnteredFunction();
				}
			}
			return;
		}
		else if (const auto* te = event.getIf<sf::Event::TextEntered>(); te) {

			wchar_t character = (wchar_t)te->unicode;

			if (character == 8) {
				// BACKSPACE
				if (!_textStr.empty()) {

					if (_cursorPosition > 0 && _editState == TextInputEditState::TextEntered) {
						_textStr.erase(_cursorPosition - 1, 1);
						_text->setString(_textStr.substr(0, _limitCharacters));
						_cursorPosition -= 1;
					}
					else {
						int min = std::min(_selectionStart, _selectionEnd);
						int max = std::max(_selectionStart, _selectionEnd);
						_textStr.erase(min, max - min);
						_text->setString(_textStr.substr(0, _limitCharacters));
						_editState = TextInputEditState::TextEntered;
						_cursorPosition = min;
						_selectionStart = -1;
						_selectionEnd = -1;
					}

					if (dataIsCorrect() && _onEditedFunction)
						_onEditedFunction();

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

				if (_editState == TextInputEditState::TextEntered) {
					_textStr.insert(_cursorPosition, c);
					_cursorPosition += 1;
				}
				else {
					int min = std::min(_selectionStart, _selectionEnd);
					int max = std::max(_selectionStart, _selectionEnd);
					_textStr.erase(min, max - min);
					_textStr.insert(min, c);
					
					_editState = TextInputEditState::TextEntered;
					_cursorPosition = min + 1;
					_selectionStart = -1;
					_selectionEnd = -1;
				}

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

				if (_cursorPosition > (int)_textStr.length())
					_cursorPosition = (int)_textStr.length();

				_text->setString(_textStr.substr(0, _limitCharacters));

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
	sf::Vector2f rectSize;
	rectSize.x = (float)(_rect.size.x - 2 * textInput_border_width);
	rectSize.y = (float)(_rect.size.y - 2 * textInput_border_width);

	sf::RectangleShape rect(rectSize);

	if (_editState == TextInputEditState::TextEntered) {
		rect.setFillColor(textinput_textentered_color);
	}
	else if (_state == TextInputState::Hover) {
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
		selectionRectSize.y = _rect.size.y - 2 * textInput_border_width - 2 * selection_margin;

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
		cursor.setPosition(_text->findCharacterPos(_cursorPosition));;
		window->draw(cursor);
	}
}