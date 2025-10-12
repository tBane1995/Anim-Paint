#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class BrightnessContrastState { Idle, Edited };

class Dialog_Brightness_Contrast : public Dialog {
public:
	BrightnessContrastState _state;

	sf::Text _brightness_text;
	sf::Text _contrast_text;
	
	Slider* _brightness_slider;
	Slider* _contrast_slider;

	NormalButtonWithText* _reset;
	NormalButtonWithText* _confirm;

	std::vector < Layer* > _original_layers;
	std::vector < Layer* > _edited_layers;

	Dialog_Brightness_Contrast(std::vector < Layer* > layers);
	~Dialog_Brightness_Contrast();

	void setPosition(sf::Vector2f position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
