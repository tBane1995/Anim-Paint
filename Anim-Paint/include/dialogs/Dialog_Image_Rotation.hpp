#pragma once
#include "Dialog.hpp"
#include "../Slider.hpp"
#include "../Layer.hpp"

enum class RotationState { Idle, Edited };

class Dialog_Image_Rotation : public Dialog {
public:

	RotationState state;

	sf::Text rotation_text;
	Slider* rotation_slider;
	NormalButtonWithText* reset;
	NormalButtonWithText* confirm;

	std::vector < Layer* > original_layers;
	std::vector < Layer* > edited_layers;

	Dialog_Image_Rotation(std::vector < Layer* > layers);
	~Dialog_Image_Rotation();

	void setPosition(sf::Vector2f position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};