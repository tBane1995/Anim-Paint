#include "Dialogs/Dialog_Outline.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"
#include "Tools/Toolbar.hpp"
#include "History.hpp"

Dialog_Outline::Dialog_Outline(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"outline", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = OutlineState::Idle;

	saveOriginalLayers(layers);

	
	_outline_slider = std::make_shared<Slider>(L"width", 0, 8, L"px");
	_outline_slider->setValue(1);
	setTheFilter();

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_outline_slider->setValue(1);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Outline::_state = OutlineState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	

	setPosition(_position);
}

Dialog_Outline::~Dialog_Outline() {
	if (Dialog_Outline::_state == OutlineState::Idle) {
		_outline_slider->setValue(0);
		setTheFilter();

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}
	else {
		// is Edited
		history->saveStep();
	}

}

void Dialog_Outline::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Outline::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2i slider_pos;
	slider_pos.x = getContentPosition().x + getContentSize().x / 2 - _outline_slider->getSize().x / 2;
	slider_pos.y = getContentPosition().y + (getContentSize().y) / 2 - _outline_slider->getSize().y / 2 - 24;
	_outline_slider->setPosition(slider_pos);

	sf::Vector2i button_pos;
	button_pos.x = _position.x + getSize().x / 2 - _confirm->getSize().x / 2;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Outline::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_outline(_edited_layers.back()->_image, _outline_slider->getValue(), toolbar->_second_color->_color, toolbar->_first_color->_color);
	}

	// TO-DO
	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
	//layers_dialog->loadLayersFromCurrentFrame();
}

void Dialog_Outline::cursorHover() {
	Dialog::cursorHover();

	_outline_slider->cursorHover();
	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Outline::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_outline_slider->handleEvent(event);
	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Outline::update() {
	Dialog::update();

	_outline_slider->update();

	if (_outline_slider->_editState == SliderEditState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Outline::draw() {
	Dialog::draw();

	_outline_slider->draw();

	_reset->draw();
	_confirm->draw();
}