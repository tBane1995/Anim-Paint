#include "Dialogs/Dialog_Invert_Colors.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Tools/Filters.hpp"
#include "Window.hpp"
#include "Components/Toolbar.hpp"
#include "History.hpp"

Dialog_Invert_Colors::Dialog_Invert_Colors(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"invert colors", sf::Vector2i(256, 160), sf::Vector2i(8, 120)) {

	_state = InvertColorsState::Idle;

	_rgb_checkbox = std::make_shared<Checkbox>(getTexture(L"tex\\btn32\\unchecked.png"), getTexture(L"tex\\btn32\\unchecked_hover.png"));
	_rgb_checkbox->addValue(getTexture(L"tex\\btn32\\checked.png"), getTexture(L"tex\\btn32\\checked_hover.png"));
	_rgb_checkbox->setValue(1);
	_rgb_checkbox->_onclick_func = [this]() {
		_rgb_checkbox->setValue(1);
		_hsv_checkbox->setValue(0);
		_palette = PaletteTypeToInvert::RGB;
		_palette_changed = true;
		};

	_hsv_checkbox = std::make_shared<Checkbox>(getTexture(L"tex\\btn32\\unchecked.png"), getTexture(L"tex\\btn32\\unchecked_hover.png"));
	_hsv_checkbox->addValue(getTexture(L"tex\\btn32\\checked.png"), getTexture(L"tex\\btn32\\checked_hover.png"));
	_hsv_checkbox->setValue(0);
	_hsv_checkbox->_onclick_func = [this]() {
		_rgb_checkbox->setValue(0);
		_hsv_checkbox->setValue(1);
		_palette = PaletteTypeToInvert::HSV;
		_palette_changed = true;
		};

	_rgb_text = std::make_unique<sf::Text>(basicFont, L"RGB", basic_text_size);
	_rgb_text->setFillColor(basic_text_color);
	_hsv_text = std::make_unique<sf::Text>(basicFont, L"HSV", basic_text_size);
	_hsv_text->setFillColor(basic_text_color);

	saveOriginalLayers(layers);

	_palette = PaletteTypeToInvert::RGB;
	_palette_changed = false;
	setTheFilter();

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_palette = PaletteTypeToInvert::RGB;
		_rgb_checkbox->setValue(1);
		_hsv_checkbox->setValue(0);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Invert_Colors::_state = InvertColorsState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	setPosition(_position);
}

Dialog_Invert_Colors::~Dialog_Invert_Colors() {
	if (Dialog_Invert_Colors::_state == InvertColorsState::Idle) {
		_palette = PaletteTypeToInvert::None;
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

void Dialog_Invert_Colors::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Invert_Colors::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	float centerX = getContentPosition().x + getContentSize().x * 0.5f;
	float centerY = getContentPosition().y + getContentSize().y * 0.5f - 24.0f;

	sf::FloatRect rgbB = _rgb_text->getLocalBounds();
	sf::FloatRect hsvB = _hsv_text->getLocalBounds();

	float padding = 24;
	float half = std::max(_rgb_checkbox->getSize().x + rgbB.size.x + padding, _hsv_checkbox->getSize().x + hsvB.size.x + padding);

	_rgb_checkbox->setPosition(sf::Vector2i((int)(centerX - half), (int)(centerY - _rgb_checkbox->getSize().y * 0.5f)));
	_rgb_text->setPosition(sf::Vector2f(centerX - half + _rgb_checkbox->getSize().x - rgbB.position.x, centerY - rgbB.position.y - rgbB.size.y * 0.5f));

	_hsv_text->setPosition(sf::Vector2f(centerX + half - hsvB.size.x - hsvB.position.x, centerY - hsvB.position.y - hsvB.size.y * 0.5f));
	_hsv_checkbox->setPosition(sf::Vector2i((int)(centerX + half - hsvB.size.x - _hsv_checkbox->getSize().x), (int)(centerY - _hsv_checkbox->getSize().y * 0.5f)));

	sf::Vector2i button_pos;
	button_pos.x = _position.x + getSize().x / 2 - _confirm->getSize().x / 2;
	button_pos.y = _position.y + 160 - _confirm->getSize().y - 16;
	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Invert_Colors::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		switch (_palette) {

			case PaletteTypeToInvert::RGB:
				set_invert_rgb(_edited_layers.back()->_image);
				break;
			case PaletteTypeToInvert::HSV:
				set_invert_hsv(_edited_layers.back()->_image);
				break;
			default:
				break;
		}
	}

	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
}

void Dialog_Invert_Colors::cursorHover() {
	Dialog::cursorHover();

	_rgb_checkbox->cursorHover();
	_hsv_checkbox->cursorHover();

	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Invert_Colors::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_rgb_checkbox->handleEvent(event);
	_hsv_checkbox->handleEvent(event);

	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Invert_Colors::update() {
	Dialog::update();

	_rgb_checkbox->update();
	_hsv_checkbox->update();

	if (_palette_changed) {
		setTheFilter();
		_palette_changed = false;
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Invert_Colors::draw() {
	Dialog::draw();

	_rgb_checkbox->draw();
	_hsv_checkbox->draw();

	window->draw(*_rgb_text);
	window->draw(*_hsv_text);

	_reset->draw();
	_confirm->draw();
}