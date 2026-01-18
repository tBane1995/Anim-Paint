#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"

enum class OutlineState { Idle, Edited };

class Dialog_Outline : public Dialog {
public:
	OutlineState _state;

	std::unique_ptr<sf::Text> _outline_text;
	std::shared_ptr<Slider> _outline_slider;
	std::shared_ptr<ColoredButtonWithText> _reset;
	std::shared_ptr<ColoredButtonWithText> _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Outline(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Outline();

	void saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers);
	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
