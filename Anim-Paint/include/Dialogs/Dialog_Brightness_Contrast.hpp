#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class BrightnessContrastState { Idle, Edited };

class Dialog_Brightness_Contrast : public Dialog {
public:
	BrightnessContrastState _state;

	sf::Text* _brightness_text;
	sf::Text* _contrast_text;
	
	Slider* _brightness_slider;
	Slider* _contrast_slider;

	ColoredButtonWithText* _reset;
	ColoredButtonWithText* _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Brightness_Contrast(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Brightness_Contrast();

	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
