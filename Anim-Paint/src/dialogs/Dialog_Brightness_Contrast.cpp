#include "Dialogs/Dialog_Brightness_Contrast.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"

Dialog_Brightness_Contrast::Dialog_Brightness_Contrast(std::vector < Layer* > layers) : 
	Dialog(L"brightness-contrast", sf::Vector2f(256, 160), sf::Vector2f(8, 120)),
	_brightness_text(basicFont, L"brightness", 13),
	_contrast_text(basicFont, L"contrast", 13) {

	_state = BrightnessContrastState::Idle;

	_brightness_slider = new Slider(-50, 50);
	_brightness_slider->setValue(0);

	_contrast_slider = new Slider(-50, 50);
	_contrast_slider->setValue(0);

	_reset = new NormalButtonWithText(L"reset", sf::Vector2f(64, 32));
	_reset->_onclick_func = [this]() {
		_brightness_slider->setValue(0);
		_contrast_slider->setValue(0);
		setTheFilter();
		};

	_confirm = new NormalButtonWithText(L"confirm", sf::Vector2f(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog_Brightness_Contrast::_state = BrightnessContrastState::Edited;
		Dialog::_state = DialogState::ToClose;

		animation->getCurrentFrame()->_layers.clear();
		animation->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};


	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(new Layer(l));
		_edited_layers.push_back(new Layer(l));
	}

	setPosition(_position);
}

Dialog_Brightness_Contrast::~Dialog_Brightness_Contrast() {

	if (Dialog_Brightness_Contrast::_state == BrightnessContrastState::Idle) {
		_brightness_slider->setValue(0);
		_contrast_slider->setValue(0);
		setTheFilter();

		animation->getCurrentFrame()->_layers.clear();
		animation->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}
}

void Dialog_Brightness_Contrast::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);

	sf::Vector2f text_pos;
	text_pos.x = int(position.x) / 8 * 8 + 24;
	text_pos.y = int(position.y) / 8 * 8 + 160 / 2 - 28;

	_brightness_text.setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2));
	_contrast_text.setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2) + sf::Vector2f(0, 32));

	sf::Vector2f slider_pos;
	slider_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 64 / 2;
	slider_pos.y = int(position.y) / 8 * 8 + 160 / 2 - 28;
	_brightness_slider->setPosition(slider_pos);
	_contrast_slider->setPosition(slider_pos + sf::Vector2f(0, 32));

	sf::Vector2f button_pos;
	button_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 32;
	button_pos.y = int(position.y) / 8 * 8 + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2f(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2f(48, 0));
}

void Dialog_Brightness_Contrast::setTheFilter() {
	for (auto& lr : _edited_layers) {
		delete lr;
	}

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(new Layer(org));
		set_brightness(_edited_layers.back()->_image, float(_brightness_slider->getValue()) / 100.0f);
		set_contrast(_edited_layers.back()->_image, float(_contrast_slider->getValue()) / 100.0f);
	}

	// TO-DO
	animation->getCurrentFrame()->_layers.clear();
	animation->getCurrentFrame()->_layers = _edited_layers;
	//layers_dialog->loadLayersFromCurrentFrame();

}

void Dialog_Brightness_Contrast::cursorHover() {
	Dialog::cursorHover();

	_brightness_slider->cursorHover();
	_contrast_slider->cursorHover();

	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Brightness_Contrast::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_brightness_slider->handleEvent(event);
	_contrast_slider->handleEvent(event);

	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Brightness_Contrast::update() {
	Dialog::update();

	_brightness_slider->update();
	_contrast_slider->update();

	if (_brightness_slider->_state == SliderState::Changed || _contrast_slider->_state == SliderState::Changed) {
		setTheFilter();
	}

	_reset->update();
	_confirm->update();

}

void Dialog_Brightness_Contrast::draw() {
	Dialog::draw();

	window->draw(_brightness_text);
	window->draw(_contrast_text);

	_brightness_slider->draw();
	_contrast_slider->draw();

	_reset->draw();
	_confirm->draw();
}
