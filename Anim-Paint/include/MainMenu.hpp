#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "ElementGUI/Button.hpp"
#include <filesystem>

class OptionBox : public ElementGUI {
public:
	sf::RectangleShape _rect;
	sf::Text _text;
	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

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
	sf::RectangleShape _rect;
	sf::Text _text;
	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;
	std::vector < OptionBox* > _options;
	bool _isOpen;

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

	sf::RectangleShape _rect;
	sf::Sprite _logo;
	std::vector < MenuBox* > _menu_boxes;
	MenuBox* _open_menu_box;

	MainMenu();
	~MainMenu();

	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);
	void saveProject(const std::filesystem::path& path);
	void loadProject(const std::filesystem::path& path);
	void exportFile(const std::filesystem::path& path);
	void importFile(const std::filesystem::path& path);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

extern MainMenu* main_menu;
