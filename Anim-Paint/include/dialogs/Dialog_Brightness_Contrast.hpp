#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class BrightnessContrastState { Idle, Edited };

class Dialog_Brightness_Contrast : public Dialog {
public:
	BrightnessContrastState state;

	sf::Text brightness_text;
	sf::Text contrast_text;
	
	Slider* brightness_slider;
	Slider* contrast_slider;

	NormalButtonWithText* reset;
	NormalButtonWithText* confirm;

	std::vector < Layer* > original_layers;
	std::vector < Layer* > edited_layers;

	Dialog_Brightness_Contrast(std::vector < Layer* > layers);
	~Dialog_Brightness_Contrast();

	void setPosition(sf::Vector2f position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};
