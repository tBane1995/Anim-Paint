#include "Dialogs/Dialog_Sepia.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"

Dialog_Sepia::Dialog_Sepia(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"sepia", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = SepiaState::Idle;

	saveOriginalLayers(layers);


	_sepia_slider = std::make_shared<BigSlider>(L"sepia", 0, 100);
	_sepia_slider->setValue(0);

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_sepia_slider->setValue(0);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Sepia::_state = SepiaState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	setPosition(_position);
}

Dialog_Sepia::~Dialog_Sepia() {
	if (Dialog_Sepia::_state == SepiaState::Idle) {
		_sepia_slider->setValue(0);
		setTheFilter();

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}

}

void Dialog_Sepia::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Sepia::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2i slider_pos;
	slider_pos.x = getContentPosition().x + getContentSize().x / 2 - _sepia_slider->getSize().x / 2;
	slider_pos.y = getContentPosition().y + (getContentSize().y) / 2 - _sepia_slider->getSize().y / 2 - 24;
	_sepia_slider->setPosition(slider_pos);

	sf::Vector2i button_pos;
	button_pos.x = _position.x + 256 / 2 - 32;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Sepia::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_sepia(_edited_layers.back()->_image, float(_sepia_slider->getValue()) / 100.0f);
	}

	// TO-DO
	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
	//layers_dialog->loadLayersFromCurrentFrame();
}

void Dialog_Sepia::cursorHover() {
	Dialog::cursorHover();

	_sepia_slider->cursorHover();
	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Sepia::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_sepia_slider->handleEvent(event);
	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Sepia::update() {
	Dialog::update();

	_sepia_slider->update();

	if (_sepia_slider->_editState == BigSliderEditState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Sepia::draw() {
	Dialog::draw();

	_sepia_slider->draw();

	_reset->draw();
	_confirm->draw();
}