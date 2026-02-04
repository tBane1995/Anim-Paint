#include "Dialogs/Dialog_Chessboard.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"
#include "Tools/Toolbar.hpp"

Dialog_Chessboard::Dialog_Chessboard(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"chessboard", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = ChessboardState::Idle;

	saveOriginalLayers(layers);

	_chessboard_text = std::make_unique<sf::Text>(basicFont, L"chessboard", 13);
	_chessboard_slider = std::make_shared<Slider>(0, 100);
	_chessboard_slider->setValue(0);

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_chessboard_slider->setValue(0);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Chessboard::_state = ChessboardState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	setPosition(_position);
}

Dialog_Chessboard::~Dialog_Chessboard() {
	if (Dialog_Chessboard::_state == ChessboardState::Idle) {
		_chessboard_slider->setValue(0);
		setTheFilter();

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
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

	sf::Vector2f text_pos;
	text_pos.x = (float)(_position.x + 24);
	text_pos.y = (float)(_position.y + dialog_title_rect_height / 2 + (160) / 2 - 24);
	_chessboard_text->setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2));

	sf::Vector2i slider_pos;
	slider_pos.x = _position.x + 256 / 2 - 64 / 2;
	slider_pos.y = _position.y + dialog_title_rect_height / 2 + (160) / 2 - 24;
	_chessboard_slider->setPosition(slider_pos);

	sf::Vector2i button_pos;
	button_pos.x = _position.x + 256 / 2 - 32;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Chessboard::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		// TO-DO
		//set_chessboard(_edited_layers.back()->_image, toolbar->_first_color->_color, toolbar->_second_color->_color);
	}

	// TO-DO
	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
	//layers_dialog->loadLayersFromCurrentFrame();
}

void Dialog_Chessboard::cursorHover() {
	Dialog::cursorHover();

	_chessboard_slider->cursorHover();
	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Chessboard::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_chessboard_slider->handleEvent(event);
	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Chessboard::update() {
	Dialog::update();

	_chessboard_slider->update();

	if (_chessboard_slider->_state == SliderState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Chessboard::draw() {
	Dialog::draw();

	window->draw(*_chessboard_text);

	_chessboard_slider->draw();

	_reset->draw();
	_confirm->draw();
}