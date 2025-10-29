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

	Button();
	~Button();

	virtual sf::Vector2i getSize();
	void select();
	void unselect();
	virtual void setPosition(sf::Vector2i position);
	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();

};

class NormalButton : public Button {
public:
	
	Texture* _texture;
	Texture* _hoverTexture;
	sf::Sprite* _sprite;
	
	ButtonState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;


	NormalButton(Texture* texture, Texture* hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
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

	std::wstring _textStr;
	std::unique_ptr < sf::Text > _text;
	sf::Color _selectColor;
	sf::Color _idleColor;
	sf::Color _hoverColor;
	sf::Color _pressColor;

	ButtonState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;


	ColoredButtonWithText(std::wstring text, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(0, 0));
	~ColoredButtonWithText();

	sf::Vector2i getSize();
	void setPosition(sf::Vector2i position);
	void setColors(sf::Color selectColor, sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor);
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

	sf::Color _rectColor;
	sf::Color _textColor;
	sf::Color _hoverTextColor;
	Texture* _texture;
	Texture* _hoverTexture;
	sf::Sprite* _sprite;
	sf::Text* _text;

	ButtonState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;


	ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, Texture* texture, Texture* hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
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
	Texture* _texture;
	Texture* _hoverTexture;
	sf::Sprite* _sprite;
	sf::Text* _text;

	ButtonState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;


	ButtonWithRightText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, Texture* texture, Texture* hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
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

	Texture* _texture;
	Texture* _hoverTexture;
	sf::Sprite* _sprite;
	sf::Text* _text;

	ButtonState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;


	Option(std::wstring text, Texture* texture, Texture* hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
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
	Texture* _texture;
	Texture* _hoverTexture;
	sf::Sprite* _sprite;
	sf::Text* _text;

	ButtonState _state;
	std::function<void()> _hover_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	sf::RectangleShape _list_rect;
	sf::VertexArray _list_border;
	std::vector < Option* > _options;
	bool _isOpen;

	ButtonWithTopTextAndList(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, sf::Vector2i position = sf::Vector2i(0, 0));
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