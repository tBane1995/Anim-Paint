#pragma once
#include "Dialog.hpp"
#include "Slider.hpp"
#include "Layer.hpp"

enum class SaturationState { Idle, Edited };

class Dialog_Image_Saturation : public Dialog {
public:
	SaturationState state;

	sf::Text saturation_text;
	Slider* saturation_slider;
	NormalButtonWithText* reset;
	NormalButtonWithText* confirm;

	std::vector < Layer* > original_layers;
	std::vector < Layer* > edited_layers;

	Dialog_Image_Saturation(std::vector < Layer* > layers);
	~Dialog_Image_Saturation();

	void setPosition(sf::Vector2f position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};
