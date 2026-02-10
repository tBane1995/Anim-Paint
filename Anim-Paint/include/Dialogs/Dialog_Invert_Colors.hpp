#pragma once
#include "Dialog.hpp"
#include "ElementGUI/Slider.hpp"
#include "Animation/Layer.hpp"
#include "ElementGUI/Checkbox.hpp"

enum class InvertColorsState { Idle, Edited };
enum class PaletteTypeToInvert { None, RGB, HSV };

class Dialog_Invert_Colors : public Dialog {
public:
	InvertColorsState _state;
	PaletteTypeToInvert _palette;
	bool _palette_changed;

	std::shared_ptr<Checkbox> _rgb_checkbox;
	std::shared_ptr<Checkbox> _hsv_checkbox;

	std::unique_ptr<sf::Text> _rgb_text;
	std::unique_ptr<sf::Text> _hsv_text;

	std::shared_ptr<ColoredButtonWithText> _reset;
	std::shared_ptr<ColoredButtonWithText> _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Invert_Colors(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Invert_Colors();

	void saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers);
	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};