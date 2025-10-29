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

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Saturation(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Saturation();

	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
