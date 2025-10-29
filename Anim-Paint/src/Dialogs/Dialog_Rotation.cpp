#include "Dialogs/Dialog_Rotation.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Window.hpp"
#include "Filters.hpp"

Dialog_Rotation::Dialog_Rotation(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"rotation", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = RotationState::Idle;
	
	_rotation_slider = new Slider(0, 359);
	_rotation_slider->setValue(0);
	_rotation_text = new sf::Text(basicFont, L"rotation", 13);

	_reset = new ColoredButtonWithText(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_rotation_slider->setValue(0);
		setTheFilter();
		};

	_confirm = new ColoredButtonWithText(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Rotation::_state = RotationState::Edited;

		animation->getCurrentFrame()->getLayers().clear();
		animation->getCurrentFrame()->getLayers() = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	_original_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}

	setPosition(_position);
}

Dialog_Rotation::~Dialog_Rotation() {
	if (Dialog_Rotation::_state == RotationState::Idle) {
		_rotation_slider->setValue(0);
		setTheFilter();

		// TO-DO
		animation->getCurrentFrame()->_layers.clear();
		animation->getCurrentFrame()->_layers = _edited_layers;
		//layers_dialog->loadLayersFromCurrentFrame();
	}

	delete _rotation_slider;
	delete _rotation_text;

	delete _reset;
	delete _confirm;

}

void Dialog_Rotation::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2f text_pos;
	text_pos.x = int(position.x) / 8 * 8 + 24;
	text_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
	_rotation_text->setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2));

	sf::Vector2i slider_pos;
	slider_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 64 / 2;
	slider_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
	_rotation_slider->setPosition(slider_pos);

	sf::Vector2i button_pos;
	button_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 32;
	button_pos.y = int(position.y) / 8 * 8 + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Rotation::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_rotation(_edited_layers.back()->_image, _rotation_slider->getValue(), true);
	}

	animation->getCurrentFrame()->_layers.clear();
	animation->getCurrentFrame()->_layers = _edited_layers;
	layers_dialog->loadLayersFromCurrentFrame();
}

void Dialog_Rotation::cursorHover() {
	Dialog::cursorHover();

	_rotation_slider->cursorHover();
	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Rotation::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_rotation_slider->handleEvent(event);
	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Rotation::update() {
	Dialog::update();

	_rotation_slider->update();

	if (_rotation_slider->_state == SliderState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Rotation::draw() {
	Dialog::draw();

	window->draw(*_rotation_text);
	_rotation_slider->draw();
	_reset->draw();
	_confirm->draw();
}