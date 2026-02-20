#include "../include/Components/Tooltip.hpp"
#include "../include/Cursor.hpp"
#include "../include/Time.hpp"

Tooltip::Tooltip(){
	_button = nullptr;
	_timer = sf::Time::Zero;

	_title = L"";
	_description = L"";
}

Tooltip::~Tooltip(){

}

void Tooltip::setButton(std::shared_ptr<Button> button){

	if (_button == button) return;

	_button = button;
	_timer = currentTime;

	//_title = _button->_title;
	//_description = _button->_description;
}

void Tooltip::handleEvent(const sf::Event& event){

	if (_button == nullptr) return;

	if (!_button->_rect.contains(cursor->_worldMousePosition)) {
		_button = nullptr;
	}
}

void Tooltip::update(){
	
}

void Tooltip::draw(){

	if (_button == nullptr) return;

	if((currentTime - _timer).asSeconds() > 0.5f){
		sf::RectangleShape rect(sf::Vector2f(100, 100));
		window->draw(rect);
	}
}


std::shared_ptr<Tooltip> tooltip;