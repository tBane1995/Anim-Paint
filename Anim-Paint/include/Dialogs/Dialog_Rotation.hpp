#pragma once
#include "Dialog.hpp"
#include "ElementGUI/BigSlider.hpp"
#include "Animation/Layer.hpp"

enum class RotationState { Idle, Edited };

class Dialog_Rotation : public Dialog {
public:

	RotationState _state;

	std::shared_ptr<BigSlider> _rotation_slider;
	std::shared_ptr<ColoredButtonWithText> _reset;
	std::shared_ptr<ColoredButtonWithText> _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Rotation(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Rotation();

	void saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers);
	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};