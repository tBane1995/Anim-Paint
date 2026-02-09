#include "Dialogs/Dialog_Rotation.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Window.hpp"
#include "Filters.hpp"
#include "Tools/Toolbar.hpp"
#include "History.hpp"

Dialog_Rotation::Dialog_Rotation(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"rotation", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = RotationState::Idle;
	
	saveOriginalLayers(layers);

	_rotation_slider = std::make_shared<Slider>(L"rotation", 0, 359, L" deg");
	_rotation_slider->setValue(0);

	_reset =std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_rotation_slider->setValue(0);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Rotation::_state = RotationState::Edited;

		getCurrentAnimation()->getCurrentFrame()->getLayers().clear();
		getCurrentAnimation()->getCurrentFrame()->getLayers() = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	setPosition(_position);
}

Dialog_Rotation::~Dialog_Rotation() {
	if (Dialog_Rotation::_state == RotationState::Idle) {
		_rotation_slider->setValue(0);
		setTheFilter();

		// TO-DO
		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		//layers_dialog->loadLayersFromCurrentFrame();
	}
	else {
		// is Edited
		history->saveStep();
	}

}

void Dialog_Rotation::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Rotation::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2i slider_pos;
	slider_pos.x = getContentPosition().x + getContentSize().x / 2 - _rotation_slider->getSize().x / 2;
	slider_pos.y = getContentPosition().y + (getContentSize().y) / 2 - _rotation_slider->getSize().y / 2 - 24;
	_rotation_slider->setPosition(slider_pos);

	sf::Vector2i button_pos;
	button_pos.x = _position.x + 256 / 2 - 32;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Rotation::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_rotation(_edited_layers.back()->_image, (float)(_rotation_slider->getValue()), true, toolbar->_second_color->_color);
	}

	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
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

	if (_rotation_slider->_editState == SliderEditState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Rotation::draw() {
	Dialog::draw();

	_rotation_slider->draw();

	_reset->draw();
	_confirm->draw();
}