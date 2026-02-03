#pragma once
#include "SFML/Graphics.hpp"
#include "ElementGUI.hpp"
#include "Textures.hpp"
#include <functional>

enum class ButtonState { Idle, Hover, Pressed };

class Button : public ElementGUI {
public:
	//sf::RectangleShape _rect;
	sf::IntRect _rect;
	bool _isSelected;
	bool _activatedByEnter;

	sf::Color _rectIdleColor;
	sf::Color _rectHoverColor;
	sf::Color _rectPressColor;
	sf::Color _rectSelectColor;

	int _rectBorderWidth;
	sf::Color _rectIdleBorderColor;
	sf::Color _rectHoverBorderColor;
	sf::Color _rectPressBorderColor;
	sf::Color _rectSelectBorderColor;

	ButtonState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	Button();
	virtual ~Button();

	void setRectColors(sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor, sf::Color selectColor);
	void setRectColors(sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor, sf::Color selectColor,
		int borderWidth, sf::Color idleBorderColor, sf::Color hoverBorderColor, sf::Color pressBorderColor, sf::Color selectBorderColor);
	virtual sf::Vector2i getSize();
	void select();
	void unselect();
	void activateByEnter(bool activate);
	virtual void setPosition(sf::Vector2i position);
	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();

};

class NormalButton : public Button {
public:

	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;

	NormalButton(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~NormalButton();

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

class ColoredButtonWithText : public Button {
public:

	std::unique_ptr < sf::Text > _text;

	ColoredButtonWithText(std::wstring text, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(0, 0));
	~ColoredButtonWithText();

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


class ButtonWithBottomText : public Button {
public:
	sf::Color _textColor;
	sf::Color _hoverTextColor;
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;
	std::unique_ptr<sf::Text> _text;

	ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~ButtonWithBottomText();

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

class ButtonWithRightText : public Button {
public:

	sf::Color _textColor;
	sf::Color _hoverTextColor;
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;
	std::unique_ptr<sf::Text> _text;

	ButtonWithRightText(std::wstring text, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~ButtonWithRightText();

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

class Option : public Button {
public:
	std::shared_ptr<Texture> _texture;
	std::shared_ptr<Texture> _hoverTexture;
	std::unique_ptr<sf::Text> _text;

	Option(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~Option();

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

	sf::Vector2i getSize();

	void addOption(std::wstring text);
	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();


	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};