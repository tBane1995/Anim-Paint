#include "Dialogs/Dialog_Hue_Saturation.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"
#include "History.hpp"

sf::Color AverageColorAlphaWeighted(const sf::Image& image, uint8_t alphaThreshold = 1)
{
	const auto size = image.getSize();

	uint64_t sumR = 0, sumG = 0, sumB = 0;
	uint64_t sumW = 0; // suma wag (alpha)

	for (unsigned y = 0; y < size.y; ++y)
	{
		for (unsigned x = 0; x < size.x; ++x)
		{
			sf::Color p = image.getPixel({ x, y });
			if (p.a < alphaThreshold) continue;

			uint64_t w = p.a; // 0..255
			sumR += uint64_t(p.r) * w;
			sumG += uint64_t(p.g) * w;
			sumB += uint64_t(p.b) * w;
			sumW += w;
		}
	}

	if (sumW == 0) return sf::Color(0, 0, 0, 0);

	uint8_t r = (uint8_t)std::clamp<uint64_t>(sumR / sumW, 0, 255);
	uint8_t g = (uint8_t)std::clamp<uint64_t>(sumG / sumW, 0, 255);
	uint8_t b = (uint8_t)std::clamp<uint64_t>(sumB / sumW, 0, 255);

	return sf::Color(r, g, b, 255);
}

Dialog_Hue_Saturation::Dialog_Hue_Saturation(std::vector<std::shared_ptr<Layer>> layers) : Dialog(L"hue - saturation", sf::Vector2i(256, 192), sf::Vector2i(8, 120)) {

	_state = HueSaturationState::Idle;

	saveOriginalLayers(layers);

	_hue_slider = std::make_shared<Slider>(L"hue", -180, 180, L" deg");
	_hue_slider->setValue(0);

	_brightness_slider = std::make_shared<Slider>(L"brightness", -50, 50);
	_brightness_slider->setValue(0);
	
	_saturation_slider = std::make_shared<Slider>(L"saturation", 0, 200, L"%");
	_saturation_slider->setValue(100);

	_reset = std::make_shared<ColoredButtonWithText>(L"reset", sf::Vector2i(64, 32));
	_reset->_onclick_func = [this]() {
		_hue_slider->setValue(0);

		_saturation_slider->setValue(100);
		setTheFilter();
		};

	_confirm = std::make_shared<ColoredButtonWithText>(L"confirm", sf::Vector2i(64, 32));
	_confirm->_onclick_func = [this]() {
		Dialog::_state = DialogState::ToClose;
		Dialog_Hue_Saturation::_state = HueSaturationState::Edited;

		getCurrentAnimation()->getCurrentFrame()->_layers.clear();
		getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	setPosition(_position);
}

Dialog_Hue_Saturation::~Dialog_Hue_Saturation() {
	if (Dialog_Hue_Saturation::_state == HueSaturationState::Idle) {
		_hue_slider->setValue(0);
		_brightness_slider->setValue(0);
		_saturation_slider->setValue(100);

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

void Dialog_Hue_Saturation::saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers)
{
	_original_layers.clear();
	_edited_layers.clear();
	for (auto& l : layers) {
		_original_layers.push_back(std::make_shared<Layer>(l));
		_edited_layers.push_back(std::make_shared<Layer>(l));
	}
}

void Dialog_Hue_Saturation::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);

	sf::Vector2i first_slider_pos;
	first_slider_pos.x = getContentPosition().x + getContentSize().x / 2 - _brightness_slider->getSize().x / 2;
	first_slider_pos.y = getContentPosition().y + 8;
	
	_hue_slider->setPosition(first_slider_pos);
	_brightness_slider->setPosition(first_slider_pos + sf::Vector2i(0, 32));
	_saturation_slider->setPosition(first_slider_pos + sf::Vector2i(0, 64));

	sf::Vector2i button_pos;
	button_pos.x = _position.x + getSize().x / 2 - _confirm->getSize().x / 2;
	button_pos.y = getContentPosition().y + getContentSize().y - _confirm->getSize().y - 16;

	_reset->setPosition(button_pos - sf::Vector2i(48, 0));
	_confirm->setPosition(button_pos + sf::Vector2i(48, 0));
}

void Dialog_Hue_Saturation::setTheFilter() {

	_edited_layers.clear();

	for (auto& org : _original_layers) {
		_edited_layers.push_back(std::make_shared<Layer>(org));
		set_hue(_edited_layers.back()->_image, _hue_slider->getValue());
		set_brightness(_edited_layers.back()->_image, _brightness_slider->getValue());
		set_saturation(_edited_layers.back()->_image, _saturation_slider->getValue());
	}

	getCurrentAnimation()->getCurrentFrame()->_layers.clear();
	getCurrentAnimation()->getCurrentFrame()->_layers = _edited_layers;
}

void Dialog_Hue_Saturation::cursorHover() {
	Dialog::cursorHover();

	_hue_slider->cursorHover();
	_brightness_slider->cursorHover();
	_saturation_slider->cursorHover();

	_reset->cursorHover();
	_confirm->cursorHover();
}

void Dialog_Hue_Saturation::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_hue_slider->handleEvent(event);
	_brightness_slider->handleEvent(event);
	_saturation_slider->handleEvent(event);

	_reset->handleEvent(event);
	_confirm->handleEvent(event);
}

void Dialog_Hue_Saturation::update() {
	Dialog::update();

	_hue_slider->update();
	_brightness_slider->update();
	_saturation_slider->update();

	if (
		_hue_slider->_editState == SliderEditState::Changed ||
		_brightness_slider->_editState == SliderEditState::Changed ||
		_saturation_slider->_editState == SliderEditState::Changed) {

		setTheFilter();
	}

	_reset->update();
	_confirm->update();
}

void Dialog_Hue_Saturation::draw() {
	Dialog::draw();

	_hue_slider->draw();
	_brightness_slider->draw();
	_saturation_slider->draw();

	sf::RectangleShape previewRect;
	sf::Vector2f position;
	position.x = getContentPosition().x + 8;
	position.y = getContentPosition().y + getContentSize().y - 48;
	previewRect.setPosition(position);
	previewRect.setSize(sf::Vector2f(32, 32));

	sf::Image& image = _edited_layers.back()->_image;
	sf::Color color = AverageColorAlphaWeighted(image);
	previewRect.setFillColor(color);
	window->draw(previewRect);

	_reset->draw();
	_confirm->draw();
}