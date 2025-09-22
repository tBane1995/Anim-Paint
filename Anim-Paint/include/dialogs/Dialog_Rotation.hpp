#pragma once
#include "Dialog.hpp"
#include "../ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class RotationState { Idle, Edited };

class Dialog_Rotation : public Dialog {
public:

	RotationState _state;

	sf::Text _rotation_text;
	Slider* _rotation_slider;
	NormalButtonWithText* _reset;
	NormalButtonWithText* _confirm;

	std::vector < Layer* > _original_layers;
	std::vector < Layer* > _edited_layers;

	Dialog_Rotation(std::vector < Layer* > layers);
	~Dialog_Rotation();

	void setPosition(sf::Vector2f position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};