#include "Dialogs/Dialog_Sepia.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Components/LayersPanel/LayersPanel.hpp"
#include "Tools/Filters.hpp"
#include "Window.hpp"
#include "History.hpp"
#include "Tools/Selection.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

Dialog_Sepia::Dialog_Sepia(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"sepia", sf::Vector2i(272, 160), sf::Vector2i(8, 120)) {

	_state = SepiaState::Idle;

	saveOriginalLayers(layers);


	_sepia_slider = std::make_shared<SliderWithButtons>(L"sepia", 0, 100, L"%");
	_sepia_slider->setValue(0);
	_sepia_slider->_slider->_onEditFunction = [this]() { setTheFilter();  };

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_sepia_slider->setValue(0);
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Sepia::_state = SepiaState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_panel->loadLayersFromCurrentFrame();
		};

	setPosition(_position);
}

Dialog_Sepia::~Dialog_Sepia() {

	if (Dialog_Sepia::_state == SepiaState::Idle) {
		_sepia_slider->setValue(0);

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _original_layers;
		layers_panel->loadLayersFromCurrentFrame();
	}
	else {
		// is Edited
		if (selection->_state == ResizableToolState::Selected) {
			sf::Image original_image = getCurrentAnimation()->getCurrentLayer()->_image;
			pasteImageWithMask(getCurrentAnimation()->getCurrentLayer()->_image, *selection->_resizedImage, selection->_resizedRect.position.x, selection->_resizedRect.position.y, *selection->_resizedMaskImage, (toolbar->_option_transparency->_checkbox->_value == 0) ? sf::Color::Transparent : toolbar->_second_color->_color);
			history->saveStep();
			canvas->_isEdited = true;
			selection->normalize(selection->_resizedRect);
			getCurrentAnimation()->getCurrentLayer()->_image = original_image;
		}
		else {
			history->saveStep();
		}
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
	button_pos.x = _position.x + getSize().x / 2 - _confirm->getSize().x / 2;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Sepia::setTheFilter() {

	if (selection->_state != ResizableToolState::None) {

		selection->resizeImage();
		set_sepia(*selection->_resizedImage, _sepia_slider->getValue());

	}
	else {
		_edited_layers.clear();
		for (auto& org : _original_layers) {
			_edited_layers.push_back(std::make_shared<Layer>(org));
		}

		set_sepia(_edited_layers[getCurrentAnimation()->getCurrentLayerID()]->_image, _sepia_slider->getValue());

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
	}
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

	_reset->update();
	_confirm->update();
}

void Dialog_Sepia::draw() {
	Dialog::draw();

	_sepia_slider->draw();

	_reset->draw();
	_confirm->draw();
}