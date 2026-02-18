#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class SmoothingState { Idle, Edited };

class Dialog_Smoothing : public Dialog {
public:
	SmoothingState _state;

	std::shared_ptr<Slider> _smoothness_slider;
	std::shared_ptr<Slider> _radius_slider;

	std::shared_ptr<ColoredButtonWithText> _reset;
	std::shared_ptr<ColoredButtonWithText> _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Smoothing(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Smoothing();

	void saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers);
	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
