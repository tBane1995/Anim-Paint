#include "../include/Components/Tooltip.hpp"
#include "../include/Cursor.hpp"
#include "../include/Time.hpp"
#include "Theme.hpp"

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
	_description = _button->_description;

	_title_text->setString(_title);
	_description_text->setString(_description);
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
			rectSize.y = int(2 * basicFont.getLineSpacing(tooltip_text_font_size) + 3 * tooltip_padding + 2 * tooltip_rect_border_width);
		else
			rectSize.y = int(basicFont.getLineSpacing(tooltip_text_font_size) + 2 * tooltip_padding + 2 * tooltip_rect_border_width);

		sf::RectangleShape rect(sf::Vector2f(rectSize.x - 2*tooltip_rect_border_width, rectSize.y - 2*tooltip_rect_border_width));
		
		rect.setFillColor(tooltip_rect_color);
		rect.setOutlineThickness(tooltip_rect_border_width);
		rect.setOutlineColor(tooltip_rect_border_color);

		sf::Vector2i pos;
		pos.x = _button->getPosition().x + _button->getSize().x / 2 + tooltip_rect_border_width;
		pos.y = _button->getPosition().y + _button->getSize().y + tooltip_rect_border_width;
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