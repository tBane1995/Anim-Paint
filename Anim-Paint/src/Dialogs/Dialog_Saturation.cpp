#include "Dialogs/Dialog_Saturation.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"

Dialog_Saturation::Dialog_Saturation(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"saturation", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = SaturationState::Idle;

	saveOriginalLayers(layers);

	
	_saturation_slider = std::make_shared<Slider>(L"saturation", 0, 200, L"%");
	_saturation_slider->setValue(100);

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_saturation_slider->setValue(100);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Saturation::_state = SaturationState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	setPosition(_position);
}

Dialog_Saturation::~Dialog_Saturation() {
	if (Dialog_Saturation::_state == SaturationState::Idle) {
		_saturation_slider->setValue(100);
		setTheFilter();

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}

}

void Dialog_Saturation::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Saturation::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2i slider_pos;
	slider_pos.x = getContentPosition().x + getContentSize().x / 2 - _saturation_slider->getSize().x / 2;
	slider_pos.y = getContentPosition().y + (getContentSize().y) / 2 - _saturation_slider->getSize().y / 2 - 24;
	_saturation_slider->setPosition(slider_pos);

	sf::Vector2i button_pos;
	button_pos.x = _position.x + 256 / 2 - 32;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Saturation::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_saturation(_edited_layers.back()->_image, float(_saturation_slider->getValue()) / 100.0f);
	}

	// TO-DO
	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
	//layers_dialog->loadLayersFromCurrentFrame();
}

void Dialog_Saturation::cursorHover() {
	Dialog::cursorHover();

	_saturation_slider->cursorHover();
	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Saturation::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_saturation_slider->handleEvent(event);
	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Saturation::update() {
	Dialog::update();

	_saturation_slider->update();

	if (_saturation_slider->_editState == SliderEditState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Saturation::draw() {
	Dialog::draw();

	_saturation_slider->draw();

	_reset->draw();
	_confirm->draw();
}