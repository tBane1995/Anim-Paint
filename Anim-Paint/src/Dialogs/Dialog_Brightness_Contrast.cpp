#include "Dialogs/Dialog_Brightness_Contrast.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"

Dialog_Brightness_Contrast::Dialog_Brightness_Contrast(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"brightness-contrast", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = BrightnessContrastState::Idle;

	_brightness_slider = new Slider(-50, 50);
	_brightness_slider->setValue(0);

	_contrast_slider = new Slider(-50, 50);
	_contrast_slider->setValue(0);

	_brightness_text = new sf::Text(basicFont, L"brightness", 13);
	_contrast_text = new sf::Text(basicFont, L"contrast", 13);

	_reset = new ColoredButtonWithText(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_brightness_slider->setValue(0);
		_contrast_slider->setValue(0);
		setTheFilter();
		};

	_confirm = new ColoredButtonWithText(L"confirm", sf::Vector2i(64, 32));
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
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
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

	delete _brightness_slider;
	delete _contrast_slider;

	delete _brightness_text;
	delete _contrast_text;

	delete _reset;
	delete _confirm;
}

void Dialog_Brightness_Contrast::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2f text_pos;
	text_pos.x = int(position.x) / 8 * 8 + 24;
	text_pos.y = int(position.y) / 8 * 8 + 160 / 2 - 28;

	_brightness_text->setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2));
	_contrast_text->setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2) + sf::Vector2f(0, 32));

	sf::Vector2i slider_pos;
	slider_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 64 / 2;
	slider_pos.y = int(position.y) / 8 * 8 + 160 / 2 - 28;
	_brightness_slider->setPosition(slider_pos);
	_contrast_slider->setPosition(slider_pos + sf::Vector2i(0, 32));

	sf::Vector2i button_pos;
	button_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 32;
	button_pos.y = int(position.y) / 8 * 8 + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Brightness_Contrast::setTheFilter() {

	_edited_layers.clear();
	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
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

	window->draw(*_brightness_text);
	window->draw(*_contrast_text);

	_brightness_slider->draw();
	_contrast_slider->draw();

	_reset->draw();
	_confirm->draw();
}
