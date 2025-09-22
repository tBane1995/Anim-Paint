#include "Dialogs/Dialog_Sepia.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"

Dialog_Sepia::Dialog_Sepia(std::vector < Layer* > layers) : Dialog(L"sepia", sf::Vector2f(256, 160), sf::Vector2f(8, 120)) {

	_state = SepiaState::Idle;

	_sepia_text = sf::Text(L"sepia", basicFont, 13);

	_sepia_slider = new Slider(0, 100);
	_sepia_slider->setValue(0);

	_reset = new NormalButtonWithText(L"reset", sf::Vector2f(64, 32));
	_reset->_onclick_func = [this]() {
		_sepia_slider->setValue(0);
		setTheFilter();
		};

	_confirm = new NormalButtonWithText(L"confirm", sf::Vector2f(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Sepia::_state = SepiaState::Edited;

		animation->getCurrentFrame()->_layers.clear();
		animation->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	_original_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(new Layer(l));
		_edited_layers.push_back(new Layer(l));
	}

	setPosition(_position);
}

Dialog_Sepia::~Dialog_Sepia() {
	if (Dialog_Sepia::_state == SepiaState::Idle) {
		_sepia_slider->setValue(0);
		setTheFilter();

		animation->getCurrentFrame()->_layers.clear();
		animation->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}
}

void Dialog_Sepia::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);

	sf::Vector2f text_pos;
	text_pos.x = int(position.x) / 8 * 8 + 24;
	text_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
	_sepia_text.setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2));

	sf::Vector2f slider_pos;
	slider_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 64 / 2;
	slider_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
	_sepia_slider->setPosition(slider_pos);

	sf::Vector2f button_pos;
	button_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 32;
	button_pos.y = int(position.y) / 8 * 8 + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2f(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2f(48, 0));
}

void Dialog_Sepia::setTheFilter() {
	for (auto& lr : _edited_layers) {
		delete lr;
	}

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(new Layer(org));
		set_sepia(_edited_layers.back()->_image, float(_sepia_slider->getValue()) / 100.0f);
	}

	// TO-DO
	animation->getCurrentFrame()->_layers.clear();
	animation->getCurrentFrame()->_layers = _edited_layers;
	//layers_dialog->loadLayersFromCurrentFrame();
}

void Dialog_Sepia::cursorHover() {
	Dialog::cursorHover();

	_sepia_slider->cursorHover();
	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Sepia::handleEvent(sf::Event& event) {
	Dialog::handleEvent(event);

	_sepia_slider->handleEvent(event);
	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Sepia::update() {
	Dialog::update();

	_sepia_slider->update();

	if (_sepia_slider->_state == SliderState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Sepia::draw() {
	Dialog::draw();

	window->draw(_sepia_text);
	_sepia_slider->draw();
	_reset->draw();
	_confirm->draw();
}