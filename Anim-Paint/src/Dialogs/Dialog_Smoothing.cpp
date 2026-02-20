#include "Dialogs/Dialog_Smoothing.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Tools/Filters.hpp"
#include "Window.hpp"
#include "Components/Toolbar.hpp"
#include "History.hpp"

Dialog_Smoothing::Dialog_Smoothing(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"smoothing", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = SmoothingState::Idle;
	saveOriginalLayers(layers);

	
	_smoothness_slider = std::make_shared<Slider>(L"smoothness", 0, 100);
	_smoothness_slider->setValue(50);

	_radius_slider = std::make_shared<Slider>(L"radius", 0, 8, L"px");
	_radius_slider->setValue(1);

	setTheFilter();

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_smoothness_slider->setValue(50);
		_radius_slider->setValue(1);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Smoothing::_state = SmoothingState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	

	setPosition(_position);
}

Dialog_Smoothing::~Dialog_Smoothing() {
	if (Dialog_Smoothing::_state == SmoothingState::Idle) {
		_smoothness_slider->setValue(0);
		_radius_slider->setValue(0);
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

void Dialog_Smoothing::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Smoothing::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2i first_slider_pos;
	first_slider_pos.x = getContentPosition().x + getContentSize().x / 2 - _smoothness_slider->getSize().x / 2;
	first_slider_pos.y = getContentPosition().y + 8;

	_smoothness_slider->setPosition(first_slider_pos);
	_radius_slider->setPosition(first_slider_pos + sf::Vector2i(0, 32));

	sf::Vector2i button_pos;
	button_pos.x = _position.x + getSize().x / 2 - _confirm->getSize().x / 2;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Smoothing::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_smoothing(_edited_layers.back()->_image, _smoothness_slider->getValue(), _radius_slider->getValue());
	}

	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
}

void Dialog_Smoothing::cursorHover() {
	Dialog::cursorHover();

	_smoothness_slider->cursorHover();
	_radius_slider->cursorHover();	
	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Smoothing::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_smoothness_slider->handleEvent(event);
	_radius_slider->handleEvent(event);
	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Smoothing::update() {
	Dialog::update();

	_smoothness_slider->update();
	_radius_slider->update();

	if (_smoothness_slider->_editState == SliderEditState::Changed || _radius_slider->_editState == SliderEditState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Smoothing::draw() {
	Dialog::draw();

	_smoothness_slider->draw();
	_radius_slider->draw();

	_reset->draw();
	_confirm->draw();
}