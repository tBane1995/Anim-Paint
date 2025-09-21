#pragma once
#include "Dialog.hpp"
#include "Slider.hpp"
#include "Layer.hpp"


enum class SepiaState { Idle, Edited };

class Dialog_Image_Sepia : public Dialog {
public:
	SepiaState state;

	sf::Text sepia_text;
	Slider* sepia_slider;
	NormalButtonWithText* reset;
	NormalButtonWithText* confirm;

	std::vector < Layer* > original_layers;
	std::vector < Layer* > edited_layers;

	Dialog_Image_Sepia(std::vector < Layer* > layers);
	~Dialog_Image_Sepia();

	void setPosition(sf::Vector2f position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};