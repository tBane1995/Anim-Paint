#pragma once
#include "Controls/Button.hpp"
#include "Controls/Option.hpp"

class ButtonWithTopTextAndList : public Button {

public:

	sf::Color _textColor;
	sf::Color _hoverTextColor;
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;
	std::unique_ptr<sf::Text> _text;

	sf::RectangleShape _list_rect;
	sf::VertexArray _list_border;
	std::vector<std::shared_ptr<Option>> _options;
	bool _isOpen;

	ButtonWithTopTextAndList(std::wstring text, sf::Color textColor, sf::Color hoverTextColor, sf::Vector2i position = sf::Vector2i(0, 0));
	~ButtonWithTopTextAndList();

	void addOption(std::shared_ptr<Option> option);
	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();


	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};