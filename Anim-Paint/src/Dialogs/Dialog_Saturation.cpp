#include "Dialogs/Dialog_Saturation.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"

Dialog_Saturation::Dialog_Saturation(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"saturation", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = SaturationState::Idle;

	_saturation_slider = new Slider(0, 200);
	_saturation_slider->setValue(100);

	_saturation_text = new sf::Text(basicFont, L"saturation", 13);

	_reset = new ColoredButtonWithText(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_saturation_slider->setValue(100);
		setTheFilter();
		};

	_confirm = new ColoredButtonWithText(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Saturation::_state = SaturationState::Edited;

		animation->getCurrentFrame()->_layers.clear();
		animation->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	_original_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}

	setPosition(_position);
}

Dialog_Saturation::~Dialog_Saturation() {
	if (Dialog_Saturation::_state == SaturationState::Idle) {
		_saturation_slider->setValue(100);
		setTheFilter();

		animation->getCurrentFrame()->_layers.clear();
		animation->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}

	delete _saturation_slider;
	delete _saturation_text;

	delete _reset;
	delete _confirm;
}

void Dialog_Saturation::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2f text_pos;
	text_pos.x = int(position.x) / 8 * 8 + 24;
	text_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
	_saturation_text->setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2));

	sf::Vector2i slider_pos;
	slider_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 64 / 2;
	slider_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
	_saturation_slider->setPosition(slider_pos);

	sf::Vector2i button_pos;
	button_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 32;
	button_pos.y = int(position.y) / 8 * 8 + 160 - _confirm->getSize().y - 16;
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
	animation->getCurrentFrame()->_layers.clear();
	animation->getCurrentFrame()->_layers = _edited_layers;
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

	if (_saturation_slider->_state == SliderState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Saturation::draw() {
	Dialog::draw();

	window->draw(*_saturation_text);
	_saturation_slider->draw();
	_reset->draw();
	_confirm->draw();
}