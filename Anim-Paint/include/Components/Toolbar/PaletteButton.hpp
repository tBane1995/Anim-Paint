#pragma once
#include "Components/Toolbar/ButtonWithBottomText.hpp"

class PaletteButton : public ButtonWithBottomText {
public:

	sf::Shader _shader;

	PaletteButton(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~PaletteButton();

	void draw();
};
