#pragma once
#include "Controls/Button.hpp"


class ButtonWithBottomText : public Button {
public:
	sf::Color _textColor;
	sf::Color _hoverTextColor;
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;
	std::unique_ptr<sf::Text> _text;

	ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~ButtonWithBottomText();

	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void draw();

};