#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class BrightnessContrastState { Idle, Edited };

class Dialog_Brightness_Contrast : public Dialog {
public:
	BrightnessContrastState _state;

	std::unique_ptr<sf::Text> _brightness_text;
	std::unique_ptr<sf::Text> _contrast_text;
	
	std::shared_ptr<Slider> _brightness_slider;
	std::shared_ptr<Slider> _contrast_slider;

	std::shared_ptr<ColoredButtonWithText> _reset;
	std::shared_ptr<ColoredButtonWithText> _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Brightness_Contrast(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Brightness_Contrast();

	void saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers);
	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
