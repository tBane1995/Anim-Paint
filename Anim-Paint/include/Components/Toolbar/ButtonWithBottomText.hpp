#pragma once
#include "Controls/Button.hpp"


class ButtonWithBottomText : public Button {
public:
	sf::Color _textColor;
	sf::Color _hoverTextColor;
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;
	std::shared_ptr<Texture> _pressTexture;
	std::unique_ptr<sf::Text> _text;

	ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, std::shared_ptr<Texture> pressTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~ButtonWithBottomText();

	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void draw();

};