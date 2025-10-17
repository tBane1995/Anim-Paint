#include "ElementGUI/TextInput.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Time.hpp"

TextInput::TextInput(sf::Vector2f size, int limitCharacters, int characterSize) :
	ElementGUI(),
	_text(basicFont, L"", characterSize)
{
	_rect = sf::RectangleShape(size);
	_rect.setFillColor(sf::Color(79, 79, 79));

	_limitCharacters = limitCharacters;

	_text.setFillColor(sf::Color(191, 191, 191));

	_cursor = sf::RectangleShape(sf::Vector2f(2, basicFont.getLineSpacing(characterSize)));
	_cursor.setFillColor(sf::Color::Red);
	_cursor.setPosition(sf::Vector2f(_text.getGlobalBounds().size.x, (_rect.getSize().y - _cursor.getSize().y) / 2.f));

	setPosition(sf::Vector2f(0, 0));
}

TextInput::~TextInput() {

}

void TextInput::setPosition(sf::Vector2f position) {
	_rect.setPosition(position);	
	_text.setPosition(position + sf::Vector2f(0, (_rect.getSize().y - basicFont.getLineSpacing(_text.getCharacterSize())) / 2.f));
	_cursor.setPosition(sf::Vector2f(_text.getPosition().x + _text.getGlobalBounds().size.x, _rect.getPosition().y + (_rect.getSize().y - _cursor.getSize().y) / 2.f));
}

void TextInput::setText(std::wstring text) {
	_text.setString(text);	
	_cursor.setPosition(sf::Vector2f(_text.getPosition().x + _text.getGlobalBounds().size.x, _rect.getPosition().y + (_rect.getSize().y - _cursor.getSize().y) / 2.f));

}

void TextInput::setLimitCharacters(int limitCharacters) {
	_limitCharacters = limitCharacters;
	_text.setString(_text.getString().substring(_limitCharacters));
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

}

void TextInput::update() {

}

void TextInput::draw() {

	window->draw(_rect);
	window->draw(_text);
	if(int(currentTime.asSeconds()*3)%2 == 0)
		window->draw(_cursor);

}


