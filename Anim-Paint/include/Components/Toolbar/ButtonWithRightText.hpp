#pragma once
#include "Controls/Button.hpp"


class ButtonWithRightText : public Button {
public:

	sf::Color _textColor;
	sf::Color _hoverTextColor;
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;
	std::shared_ptr<Texture> _pressTexture;
	std::unique_ptr<sf::Text> _text;

	ButtonWithRightText(std::wstring text, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, std::shared_ptr<Texture> pressTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~ButtonWithRightText();

	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void draw();

};