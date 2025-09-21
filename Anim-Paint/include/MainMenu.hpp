#pragma once
#include "ElementGUI.hpp"
#include "Button.hpp"

class OptionBox : public ElementGUI {
public:
	sf::RectangleShape rect;
	sf::Text text;
	ButtonState state;
	std::function<void()> onclick_func;
	sf::Time clickTime;

	OptionBox(std::wstring text);
	~OptionBox();

	void setPosition(sf::Vector2f position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

class MenuBox : public ElementGUI {
public:
	sf::RectangleShape rect;
	sf::Text text;
	ButtonState state;
	std::function<void()> onclick_func;
	sf::Time clickTime;
	std::vector < OptionBox* > options;
	bool isOpen;

	MenuBox(std::wstring text);
	~MenuBox();

	void addOption(OptionBox* option);
	void setPosition(sf::Vector2f position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

class MainMenu : public ElementGUI {
public:

	sf::RectangleShape rect;
	sf::Sprite logo;
	std::vector < MenuBox* > menu_boxes;
	MenuBox* open_menu_box;

	MainMenu();
	~MainMenu();

	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

extern MainMenu* main_menu;
