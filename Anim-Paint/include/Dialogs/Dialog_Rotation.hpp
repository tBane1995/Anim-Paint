#pragma once
#include "Dialog.hpp"
#include "../ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class RotationState { Idle, Edited };

class Dialog_Rotation : public Dialog {
public:

	RotationState _state;

	sf::Text* _rotation_text;
	Slider* _rotation_slider;
	ColoredButtonWithText* _reset;
	ColoredButtonWithText* _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Rotation(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Rotation();

	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};