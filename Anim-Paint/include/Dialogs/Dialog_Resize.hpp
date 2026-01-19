#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"
#include "ElementGUI/NumberInput.hpp"

enum class ResizeState { Idle, Edited };

class Dialog_Resize : public Dialog {
public:
	ResizeState _state;

	std::unique_ptr<sf::Text> _width_text;
	std::unique_ptr<sf::Text> _height_text;
	std::unique_ptr<sf::Text> _width_percent_text;
	std::unique_ptr<sf::Text> _height_percent_text;

	std::shared_ptr<NumberInput> _width_input;
	std::shared_ptr<NumberInput> _height_input;
	std::shared_ptr<NumberInput> _width_percent_input;
	std::shared_ptr<NumberInput> _height_percent_input;

	std::shared_ptr<ColoredButtonWithText> _reset;
	std::shared_ptr<ColoredButtonWithText> _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Resize(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Resize();

	void saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers);
	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
