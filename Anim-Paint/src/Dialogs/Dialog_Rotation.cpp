#include "Dialogs/Dialog_Rotation.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Window.hpp"
#include "Tools/Filters.hpp"
#include "Components/Toolbar.hpp"
#include "History.hpp"

Dialog_Rotation::Dialog_Rotation(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"rotation", sf::Vector2i(256, 192), sf::Vector2i(8, 120)) {

	_state = RotationState::Idle;
	
	saveOriginalLayers(layers);

	_rotation_slider = std::make_shared<Slider>(L"rotation", 0, 359, L" deg");
	_rotation_slider->setValue(0);

	_smoothness_slider = std::make_shared<Slider>(L"smoothness", 0, 100);
	_smoothness_slider->setValue(100);

	_radius_slider = std::make_shared<Slider>(L"radius", 0, 8, L"px");
	_radius_slider->setValue(1);

	setTheFilter();

	_reset =std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_rotation_slider->setValue(0);
		_smoothness_slider->setValue(100);
		_radius_slider->setValue(1);
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
		_smoothness_slider->setValue(0);
		_radius_slider->setValue(0);

		setTheFilter();

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
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

	sf::Vector2i first_slider_pos;
	first_slider_pos.x = getContentPosition().x + getContentSize().x / 2 - _rotation_slider->getSize().x / 2;
	first_slider_pos.y = getContentPosition().y + 8;

	_rotation_slider->setPosition(first_slider_pos);
	_smoothness_slider->setPosition(first_slider_pos + sf::Vector2i(0, 32));
	_radius_slider->setPosition(first_slider_pos + sf::Vector2i(0, 64));

	sf::Vector2i button_pos;
	button_pos.x = _position.x + getSize().x / 2 - _confirm->getSize().x / 2;
	button_pos.y = getContentPosition().y + getContentSize().y - _confirm->getSize().y - 16;

	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Rotation::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_rotation(_edited_layers.back()->_image, (float)(_rotation_slider->getValue()), true, toolbar->_second_color->_color);
		set_smoothing(_edited_layers.back()->_image, _smoothness_slider->getValue(), _radius_slider->getValue());
	}

	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
	layers_dialog->loadLayersFromCurrentFrame();
}

void Dialog_Rotation::cursorHover() {
	Dialog::cursorHover();

	_rotation_slider->cursorHover();
	_smoothness_slider->cursorHover();
	_radius_slider->cursorHover();

	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Rotation::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_rotation_slider->handleEvent(event);
	_smoothness_slider->handleEvent(event);
	_radius_slider->handleEvent(event);

	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Rotation::update() {
	Dialog::update();

	_rotation_slider->update();
	_smoothness_slider->update();
	_radius_slider->update();

	if (_rotation_slider->_editState == SliderEditState::Changed || _smoothness_slider->_editState == SliderEditState::Changed || _radius_slider->_editState == SliderEditState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Rotation::draw() {
	Dialog::draw();

	_rotation_slider->draw();
	_smoothness_slider->draw();
	_radius_slider->draw();
	
	_reset->draw();
	_confirm->draw();
}