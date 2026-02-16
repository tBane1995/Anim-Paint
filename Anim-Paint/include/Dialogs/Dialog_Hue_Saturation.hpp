#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class HueSaturationState { Idle, Edited };

class Dialog_Hue_Saturation : public Dialog {
public:
	HueSaturationState _state;

	std::shared_ptr<Slider> _hue_slider;
	std::shared_ptr<Slider> _brightness_slider;
	std::shared_ptr<Slider> _saturation_slider;

	std::shared_ptr<ColoredButtonWithText> _reset;
	std::shared_ptr<ColoredButtonWithText> _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Hue_Saturation(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Hue_Saturation();

	void saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers);
	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
