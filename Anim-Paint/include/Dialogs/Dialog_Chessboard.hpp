#pragma once
#include "Dialog.hpp"
#include "ElementGUI/BigSlider.hpp"
#include "Animation/Layer.hpp"


enum class ChessboardState { Idle, Edited };

class Dialog_Chessboard : public Dialog {
public:
	ChessboardState _state;

	std::shared_ptr<BigSlider> _tileCount_slider;

	std::shared_ptr<BigSlider> _transparency_slider;

	std::shared_ptr<ColoredButtonWithText> _reset;
	std::shared_ptr<ColoredButtonWithText> _confirm;

	std::vector<std::shared_ptr<Layer>> _original_layers;
	std::vector<std::shared_ptr<Layer>> _edited_layers;

	Dialog_Chessboard(std::vector<std::shared_ptr<Layer>> layers);
	~Dialog_Chessboard();

	void saveOriginalLayers(std::vector<std::shared_ptr<Layer>> layers);
	void setPosition(sf::Vector2i position);
	void setTheFilter();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};