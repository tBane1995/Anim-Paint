#pragma once
#include "Controls/Button.hpp"


class Option : public Button {
public:
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;
	std::unique_ptr<sf::Text> _text;

	Option(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~Option();

	void setPosition(sf::Vector2i position);

	void draw();

};
