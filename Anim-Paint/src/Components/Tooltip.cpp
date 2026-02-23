#include "../include/Components/Tooltip.hpp"
#include "../include/Cursor.hpp"
#include "../include/Time.hpp"
#include "Theme.hpp"

std::wstring wrap_text(int line_width, sf::Font& font, float characterSize, std::wstring text) {

	std::wstring wrapped_text = L"";

	std::wstring line = L"";
	std::wstring word = L"";

	for (auto& character : text) {

		std::shared_ptr<sf::Text> text_word = std::make_shared<sf::Text>(font, word + character, characterSize);

		if (text_word->getGlobalBounds().size.x > line_width) {

			if (line != L"") {
				wrapped_text += line + L"\n";
				line = L"";
			}

			// word longer than line
			std::wstring l = L"";
			word = word + character;
			for (wchar_t& c : word) {
				std::shared_ptr<sf::Text> text_line_plus_character = std::make_shared<sf::Text>(font, l + c, characterSize);
				if (text_line_plus_character->getGlobalBounds().size.x > line_width) {
					wrapped_text += l + L"\n";
					l = c;
				}
				else
					l = l + c;
			}

			std::shared_ptr<sf::Text> text_line = std::make_shared<sf::Text>(font, l, characterSize);
			wrapped_text += l + L"\n";

			word = L"";
			continue;
		}
		
		std::shared_ptr<sf::Text> text_line_plus_word_plus_character = std::make_shared<sf::Text>(font, line + word + character, characterSize);
		if (text_line_plus_word_plus_character->getGlobalBounds().size.x > line_width)
		{
			wrapped_text += line + L"\n";
			line = L"";
			word = word + character;
			continue;
		}
		
		if (character == L'\n') {

			std::shared_ptr<sf::Text> text_line_plus_word = std::make_shared<sf::Text>(font, line + word, characterSize);
			if (text_line_plus_word->getGlobalBounds().size.x > line_width) {
				wrapped_text += line + L"\n";
				wrapped_text += word + L"\n";

				line = L"";
				word = L"";
			}
			else {
				wrapped_text += line + word + L"\n";
				line = L"";
				word = L"";
			}

			continue;
		}

		if (character == L' ' || character == L'\t') {

			std::shared_ptr<sf::Text> text_line_plus_word = std::make_shared<sf::Text>(font, line + word, characterSize);

			if (text_line_plus_word->getGlobalBounds().size.x > line_width) {
				wrapped_text += line + L"\n";
				line = L"";
			}
			else {
				line = line + word + character;
			}

			word = L"";
		}
		else {
			word = word + character;
		}
	}

	if (line != L"" || word != L"") {
		wrapped_text += line + word;
	}

	return wrapped_text;

}



Tooltip::Tooltip(){
	_button = nullptr;
	_timer = sf::Time::Zero;

	_title = L"";
	_description = L"";

	_title_text = std::make_shared<sf::Text>(basicFont, _title, tooltip_text_font_size);
	_title_text->setStyle(sf::Text::Bold);
	_title_text->setFillColor(tooltip_text_color);

	_description_text = std::make_shared<sf::Text>(basicFont, _description, tooltip_text_font_size);
	_description_text->setFillColor(tooltip_text_color);
}

Tooltip::~Tooltip(){

}

void Tooltip::setButton(std::shared_ptr<Button> button){

	if (_button == button) return;

	_button = button;
	_timer = currentTime;

	_title = _button->_title;
	_description = wrap_text(192, basicFont, tooltip_text_font_size, _button->_description);

	_title_text->setString(_title);
	_description_text->setString(_description);
}

float Tooltip::getHeightOfTitleAndDescription() {

	float height = 0.f;
	
	if (_title != L"") height += basicFont.getLineSpacing(tooltip_text_font_size);

	for(wchar_t c : _title) {
		if (c == L'\n') height += basicFont.getLineSpacing(tooltip_text_font_size);
	}

	if (_description != L"") height += basicFont.getLineSpacing(tooltip_text_font_size);

	for (wchar_t c : _description) {
		if (c == L'\n') height += basicFont.getLineSpacing(tooltip_text_font_size);
	}

	return height;

}

void Tooltip::update(){
	if (_button == nullptr) return;

	if (!_button->_rect.contains(cursor->_worldMousePosition)) {
		_button = nullptr;
	}
}

void Tooltip::draw(){

	if (_button == nullptr) return;

	if((currentTime - _timer).asSeconds() > 0.5f && (_title != L"" || _description != L"")) {

		sf::Vector2i rectSize;
		rectSize.x = (_title_text->getGlobalBounds().size.x > _description_text->getGlobalBounds().size.x) ? int(_title_text->getGlobalBounds().size.x) : int(_description_text->getGlobalBounds().size.x);
		rectSize.x += 2 * tooltip_padding + 2 * tooltip_rect_border_width;
		
		if (_title != L"" && _description != L"")
			rectSize.y = int(getHeightOfTitleAndDescription() + 3 * tooltip_padding + 2 * tooltip_rect_border_width);
		else
			rectSize.y = int(getHeightOfTitleAndDescription() + 2 * tooltip_padding + 2 * tooltip_rect_border_width);


		sf::RectangleShape rect(sf::Vector2f(rectSize.x - 2*tooltip_rect_border_width, rectSize.y - 2*tooltip_rect_border_width));
		
		rect.setFillColor(tooltip_rect_color);
		rect.setOutlineThickness(tooltip_rect_border_width);
		rect.setOutlineColor(tooltip_rect_border_color);

		sf::Vector2i pos;
		pos.x = _button->getPosition().x + _button->getSize().x / 2 + tooltip_rect_border_width;
		pos.y = _button->getPosition().y + _button->getSize().y + tooltip_rect_border_width;
		pos.x = std::clamp(pos.x, 0, int(window->getSize().x - rectSize.x));
		pos.y = std::clamp(pos.y, 0, int(window->getSize().y - rectSize.y));
		rect.setPosition(sf::Vector2f(pos));
	
		window->draw(rect);

		if (_title != L"") {
			sf::Vector2i titlePos;
			titlePos.x = pos.x + tooltip_padding;
			titlePos.y = pos.y + tooltip_padding;
			_title_text->setPosition(sf::Vector2f(titlePos));
			window->draw(*_title_text);
		}

		if (_description != L"") {
			sf::Vector2i descriptionPos;
			descriptionPos.x = pos.x + tooltip_padding;
			descriptionPos.y = pos.y + tooltip_padding;
			if (_title != L"") descriptionPos.y += basicFont.getLineSpacing(tooltip_text_font_size) + tooltip_padding;
			_description_text->setPosition(sf::Vector2f(descriptionPos));
			window->draw(*_description_text);
		}
	}
}


std::shared_ptr<Tooltip> tooltip;