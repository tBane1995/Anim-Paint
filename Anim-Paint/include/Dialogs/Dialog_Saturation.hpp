#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class SaturationState { Idle, Edited };

class Dialog_Saturation : public Dialog {
public:
	SaturationState _state;

	sf::Text* _saturation_text;
	Slider* _saturation_slider;
	ColoredButtonWithText* _reset;
	ColoredButtonWithText* _confirm;

	std::vector < Layer* > _original_layers;
	std::vector < Layer* > _edited_layers;

	Dialog_Saturation(std::vector < Layer* > layers);
	~Dialog_Saturation();

	void setPosition(sf::Vector2f position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
