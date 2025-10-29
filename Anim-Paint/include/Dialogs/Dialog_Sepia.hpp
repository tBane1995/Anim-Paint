#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"


enum class SepiaState { Idle, Edited };

class Dialog_Sepia : public Dialog {
public:
	SepiaState _state;

	sf::Text* _sepia_text;
	Slider* _sepia_slider;
	ColoredButtonWithText* _reset;
	ColoredButtonWithText* _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Sepia(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Sepia();

	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};