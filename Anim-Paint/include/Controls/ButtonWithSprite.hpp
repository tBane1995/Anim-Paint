#pragma once
#include "Controls/Button.hpp"

class ButtonWithSprite : public Button{
public:

	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;

	ButtonWithSprite(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~ButtonWithSprite();

	sf::Vector2i getSize();
	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
