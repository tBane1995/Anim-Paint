#include "Dialogs/Dialog_Image_Saturation.hpp"
#include "Theme.hpp"
#include "Animation/Animation.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Filters.hpp"
#include "Window.hpp"

Dialog_Image_Saturation::Dialog_Image_Saturation(std::vector < Layer* > layers) : Dialog(L"saturation", sf::Vector2f(256, 160), sf::Vector2f(8, 120)) {

	state = SaturationState::Idle;

	saturation_text = sf::Text(L"saturation", basicFont, 13);

	saturation_slider = new Slider(0, 200);
	saturation_slider->setValue(100);

	reset = new NormalButtonWithText(L"reset", sf::Vector2f(64, 32));
	reset->onclick_func = [this]() {
		saturation_slider->setValue(100);
		setTheFilter();
		};

	confirm = new NormalButtonWithText(L"confirm", sf::Vector2f(64, 32));
	confirm->onclick_func = [this]() {
		Dialog::state = DialogState::ToClose;
		Dialog_Image_Saturation::state = SaturationState::Edited;

		animation->getCurrentFrame()->layers.clear();
		animation->getCurrentFrame()->layers = edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
		};

	original_layers.clear();
	for (auto& l : layers) {
		original_layers.push_back(new Layer(l));
		edited_layers.push_back(new Layer(l));
	}

	setPosition(position);
}

Dialog_Image_Saturation::~Dialog_Image_Saturation() {
	if (Dialog_Image_Saturation::state == SaturationState::Idle) {
		saturation_slider->setValue(100);
		setTheFilter();

		animation->getCurrentFrame()->layers.clear();
		animation->getCurrentFrame()->layers = edited_layers;
		layers_dialog->loadLayersFromCurrentFrame();
	}
}

void Dialog_Image_Saturation::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);

	sf::Vector2f text_pos;
	text_pos.x = int(position.x) / 8 * 8 + 24;
	text_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
	saturation_text.setPosition(text_pos + sf::Vector2f(0, 2 - basicFont.getLineSpacing(13) / 2));

	sf::Vector2f slider_pos;
	slider_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 64 / 2;
	slider_pos.y = int(position.y) / 8 * 8 + dialog_title_rect_height / 2 + (160) / 2 - 24;
	saturation_slider->setPosition(slider_pos);

	sf::Vector2f button_pos;
	button_pos.x = int(position.x) / 8 * 8 + 256 / 2 - 32;
	button_pos.y = int(position.y) / 8 * 8 + 160 - confirm->getSize().y - 16;
	reset->setPosition(button_pos - sf::Vector2f(48, 0));
	confirm->setPosition(button_pos + sf::Vector2f(48, 0));
}

void Dialog_Image_Saturation::setTheFilter() {
	for (auto& lr : edited_layers) {
		delete lr;
	}

	edited_layers.clear();

	for (auto& org : original_layers) {
		edited_layers.push_back(new Layer(org));
		set_saturation(edited_layers.back()->image, float(saturation_slider->getValue()) / 100.0f);
	}

	// TO-DO
	animation->getCurrentFrame()->layers.clear();
	animation->getCurrentFrame()->layers = edited_layers;
	//layers_dialog->loadLayersFromCurrentFrame();
}

void Dialog_Image_Saturation::cursorHover() {
	Dialog::cursorHover();

	this->saturation_slider->cursorHover();
	reset->cursorHover();
	confirm->cursorHover();
}

void Dialog_Image_Saturation::handleEvent(sf::Event& event) {
	Dialog::handleEvent(event);

	this->saturation_slider->handleEvent(event);
	reset->handleEvent(event);
	confirm->handleEvent(event);
}

void Dialog_Image_Saturation::update() {
	Dialog::update();

	saturation_slider->update();

	if (saturation_slider->state == SliderState::Changed) {

		setTheFilter();
	}

	reset->update();
	confirm->update();
}

void Dialog_Image_Saturation::draw() {
	Dialog::draw();

	window->draw(saturation_text);
	saturation_slider->draw();
	reset->draw();
	confirm->draw();
}