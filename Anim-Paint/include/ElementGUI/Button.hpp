#pragma once
#include "SFML/Graphics.hpp"
#include "ElementGUI.hpp"
#include "Textures.hpp"
#include <functional>

enum class ButtonState { Idle, Hover, Pressed };

class Button : public ElementGUI {
public:
	sf::RectangleShape rect;
	
	Button();
	~Button();

	virtual sf::Vector2f getSize();
	virtual void setPosition(sf::Vector2f position);
	virtual void cursorHover();
	virtual void handleEvent(sf::Event& event);
	virtual void update();
	virtual void draw();

};

class NormalButton : public Button {
public:
	
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;

	sf::Vector2f position;
	
	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	NormalButton(Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0, 0));
	~NormalButton();

	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};

class NormalButtonWithText : public Button {
public:

	sf::RectangleShape rect;
	sf::Text text;
	sf::Color normal_color;
	sf::Color hover_color;
	sf::Color press_color;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	NormalButtonWithText(std::wstring text, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(0, 0));
	~NormalButtonWithText();

	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};


class ButtonWithBottomText : public Button {
public:

	sf::Color textColor;
	sf::Color hoverTextColor;
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;
	sf::Text text;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	ButtonWithBottomText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0, 0));
	~ButtonWithBottomText();

	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);
	void unclick();
	void hover();
	void click();


	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};

class ButtonWithRightText : public Button {
public:

	sf::Color textColor;
	sf::Color hoverTextColor;
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;
	sf::Text text;
	sf::RectangleShape rect;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	ButtonWithRightText(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0, 0));
	~ButtonWithRightText();

	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};

class Option : public Button {
public:

	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;
	sf::Text text;
	sf::RectangleShape rect;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;


	Option(std::wstring text, Texture* texture, Texture* hoverTexture, sf::Vector2f position = sf::Vector2f(0, 0));
	~Option();

	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};

class ButtonWithTopTextAndList : public Button {
public:

	sf::Color textColor;
	sf::Color hoverTextColor;
	Texture* texture;
	Texture* hoverTexture;
	sf::Sprite sprite;
	sf::Text text;
	sf::RectangleShape rect;

	sf::Vector2f position;

	ButtonState state;
	std::function<void()> hover_func;
	std::function<void()> onclick_func;
	sf::Time clickTime;

	sf::RectangleShape list_rect;
	sf::VertexArray list_border;
	std::vector < Option* > options;
	bool isOpen;

	ButtonWithTopTextAndList(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, sf::Vector2f position = sf::Vector2f(0, 0));
	~ButtonWithTopTextAndList();

	sf::Vector2f getSize();

	void addOption(std::wstring text);
	void setPosition(sf::Vector2f position);
	void unclick();
	void hover();
	void click();


	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};
