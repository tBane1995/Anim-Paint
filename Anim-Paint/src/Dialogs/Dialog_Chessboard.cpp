#include "Dialogs/Dialog_Chessboard.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "../include/Components/LayersPanel.hpp"
#include "Tools/Filters.hpp"
#include "Window.hpp"
#include "Components/Toolbar.hpp"
#include "History.hpp"

Dialog_Chessboard::Dialog_Chessboard(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"chessboard", sf::Vector2i(256+16, 160), sf::Vector2i(8, 120)) {

	_state = ChessboardState::Idle;

	saveOriginalLayers(layers);

	int maxTileCount = std::max(getCurrentAnimation()->getCurrentLayer()->_image.getSize().x, getCurrentAnimation()->getCurrentLayer()->_image.getSize().y);
	_tileCount_slider = std::make_shared<Slider>(L"tile count",1, maxTileCount);
	_tileCount_slider->setValue(2);

	_transparency_slider = std::make_shared<Slider>(L"transparency", 0, 100, L"%");
	_transparency_slider->setValue(20);

	setTheFilter();

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_tileCount_slider->setValue(2);
		_transparency_slider->setValue(20);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Chessboard::_state = ChessboardState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_panel->loadLayersFromCurrentFrame();
		};

	setPosition(_position);
}

Dialog_Chessboard::~Dialog_Chessboard() {
	if (Dialog_Chessboard::_state == ChessboardState::Idle) {
		_tileCount_slider->setValue(1);
		_transparency_slider->setValue(0);
		setTheFilter();

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_panel->loadLayersFromCurrentFrame();
	}
	else {
		// is Edited
		history->saveStep();
	}

}

void Dialog_Chessboard::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Chessboard::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2i slider_pos;
	slider_pos.x = getContentPosition().x + getContentSize().x / 2 - _tileCount_slider->getSize().x / 2;
	slider_pos.y = getContentPosition().y + (getContentSize().y) / 2 - _tileCount_slider->getSize().y - 28;
	_tileCount_slider->setPosition(slider_pos);
	_transparency_slider->setPosition(slider_pos + sf::Vector2i(0, 32));

	sf::Vector2i button_pos;
	button_pos.x = _position.x + getSize().x / 2 - _confirm->getSize().x / 2;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Chessboard::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_chessboard(_edited_layers.back()->_image, _tileCount_slider->getValue(), _transparency_slider->getValue(), toolbar->_first_color->_color, toolbar->_second_color->_color);
	}

	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
}

void Dialog_Chessboard::cursorHover() {
	Dialog::cursorHover();

	_tileCount_slider->cursorHover();
	_transparency_slider->cursorHover();
	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Chessboard::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_tileCount_slider->handleEvent(event);
	_transparency_slider->handleEvent(event);

	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Chessboard::update() {
	Dialog::update();

	_tileCount_slider->update();
	_transparency_slider->update();

	if (_tileCount_slider->_editState == SliderEditState::Changed || _transparency_slider->_editState == SliderEditState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Chessboard::draw() {
	Dialog::draw();

	_tileCount_slider->draw();
	_transparency_slider->draw();

	_reset->draw();
	_confirm->draw();
}