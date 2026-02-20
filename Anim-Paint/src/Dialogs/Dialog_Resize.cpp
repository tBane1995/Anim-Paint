#include "Dialogs/Dialog_Resize.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Tools/Filters.hpp"
#include "Window.hpp"
#include "Tools/Toolbar.hpp"
#include "Canvas.hpp"
#include "History.hpp"

Dialog_Resize::Dialog_Resize(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"resize", sf::Vector2i(256, 160+64), sf::Vector2i(8, 120)) {

	_state = ResizeState::Idle;

	saveOriginalLayers(layers);

	_width_input = std::make_shared<NumberInput>(sf::Vector2i(64, basic_text_rect_height), 3, basic_text_size, canvas->_size.x, canvas->_minSize.x, canvas->_maxSize.x);
	_height_input = std::make_shared<NumberInput>(sf::Vector2i(64, basic_text_rect_height), 3, basic_text_size, canvas->_size.y, canvas->_minSize.y, canvas->_maxSize.y);
	_width_percent_input = std::make_shared<NumberInput>(sf::Vector2i(64, basic_text_rect_height), 3, basic_text_size, 100, 25, 400);
	_height_percent_input = std::make_shared<NumberInput>(sf::Vector2i(64, basic_text_rect_height), 3, basic_text_size, 100, 25, 400);

	_width_input->_onEditedFunction = [this]() {
		int width = _width_input->getValue();
		_width_percent_input->setValue(int((float(width) / float(canvas->_size.x)) * 100.0f));
		setTheFilter();
		};

	_height_input->_onEditedFunction = [this]() {
		int height = _height_input->getValue();
		_height_percent_input->setValue(int((float(height) / float(canvas->_size.y)) * 100.0f));
		setTheFilter();
		};

	_width_percent_input->_onEditedFunction = [this]() {
		int width_percent = _width_percent_input->getValue();
		_width_input->setValue(int((float(width_percent) / 100.0f) * float(canvas->_size.x)));
		setTheFilter();
		};

	_height_percent_input->_onEditedFunction = [this]() {
		int height_percent = _height_percent_input->getValue();
		_height_input->setValue(int((float(height_percent) / 100.0f) * float(canvas->_size.y)));
		setTheFilter();
		};


	_width_text = std::make_unique<sf::Text>(basicFont, "Width (px)", basic_text_size);
	_height_text = std::make_unique<sf::Text>(basicFont, "Height (px)", basic_text_size);
	_width_percent_text = std::make_unique<sf::Text>(basicFont, "Width (%)", basic_text_size);
	_height_percent_text = std::make_unique<sf::Text>(basicFont, "Heigth (%)", basic_text_size);

	setTheFilter();

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_width_input->setValue(canvas->_size.x);
		_height_input->setValue(canvas->_size.y);
		_width_percent_input->setValue(100);
		_height_percent_input->setValue(100);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Resize::_state = ResizeState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	

	setPosition(_position);
}

Dialog_Resize::~Dialog_Resize() {
	if (Dialog_Resize::_state == ResizeState::Idle) {
		_width_input->setValue(canvas->_size.x);
		_height_input->setValue(canvas->_size.y);
		_width_percent_input->setValue(100);
		_height_percent_input->setValue(100);
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

void Dialog_Resize::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Resize::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	// draw texts inputs
	sf::Vector2i size(64, basic_text_rect_height);

	int dy = 16;
	int distYToCenter = 28;
	int distXToCenter = 48;

	sf::Vector2i center;
	center.x = getContentPosition().x + getContentSize().x / 2;
	center.y = getContentPosition().y + getContentSize().y / 2 - dy;

	_width_input->setPosition(center + sf::Vector2i(-distXToCenter - size.x / 2, -distYToCenter - size.y / 2));
	_height_input->setPosition(center + sf::Vector2i(distXToCenter - size.x / 2, -distYToCenter - size.y / 2));
	_width_percent_input->setPosition(center + sf::Vector2i(-distXToCenter - size.x / 2, distYToCenter - size.y / 2));
	_height_percent_input->setPosition(center + sf::Vector2i(distXToCenter - size.x / 2, distYToCenter - size.y / 2));

	sf::Vector2f width_text_pos;
	width_text_pos.x = (float)(_width_input->getPosition().x + size.x / 2) - _width_text->getGlobalBounds().size.x/2.0f;
	width_text_pos.y = (float)(_width_input->getPosition().y - basic_text_rect_height);
	_width_text->setPosition(width_text_pos);

	sf::Vector2f height_text_pos;
	height_text_pos.x = (float)(_height_input->getPosition().x + size.x / 2) - _height_text->getGlobalBounds().size.x / 2.0f;
	height_text_pos.y = (float)(_height_input->getPosition().y - basic_text_rect_height);
	_height_text->setPosition(height_text_pos);

	sf::Vector2f width_percent_text_pos;
	width_percent_text_pos.x = (float)(_width_percent_input->getPosition().x + size.x / 2) - _width_percent_text->getGlobalBounds().size.x / 2.0f;
	width_percent_text_pos.y = (float)(_width_percent_input->getPosition().y - basic_text_rect_height);
	_width_percent_text->setPosition(width_percent_text_pos);

	sf::Vector2f height_percent_text_pos;
	height_percent_text_pos.x = (float)(_height_percent_input->getPosition().x + size.x / 2) - _height_percent_text->getGlobalBounds().size.x / 2.0f;
	height_percent_text_pos.y = (float)(_height_percent_input->getPosition().y - basic_text_rect_height);
	_height_percent_text->setPosition(height_percent_text_pos);

	sf::Vector2i button_pos;
	button_pos.x = _position.x + getSize().x / 2 - _confirm->getSize().x / 2;
	button_pos.y = _position.y + 160 + 64 - _confirm->getSize().y - 16;

	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Resize::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_resize(_edited_layers.back()->_image, _width_input->getValue(), _height_input->getValue(), toolbar->_second_color->_color);
	}

	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
}

void Dialog_Resize::cursorHover() {
	Dialog::cursorHover();

	//_outline_slider->cursorHover();
	_width_input->cursorHover();
	_height_input->cursorHover();
	_width_percent_input->cursorHover();
	_height_percent_input->cursorHover();

	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Resize::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	//_outline_slider->handleEvent(event);
	_width_input->handleEvent(event);
	_height_input->handleEvent(event);
	_width_percent_input->handleEvent(event);
	_height_percent_input->handleEvent(event);

	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Resize::update() {
	Dialog::update();

	_width_input->update();
	_height_input->update();
	_width_percent_input->update();
	_height_percent_input->update();

	_reset->update();
	_confirm->update();
}

void Dialog_Resize::draw() {
	Dialog::draw();

	//window->draw(*_outline_text);

	//_outline_slider->draw();

	_width_input->draw();
	_height_input->draw();
	_width_percent_input->draw();
	_height_percent_input->draw();

	window->draw(*_width_text);
	window->draw(*_height_text);
	window->draw(*_width_percent_text);
	window->draw(*_height_percent_text);

	_reset->draw();
	_confirm->draw();
}