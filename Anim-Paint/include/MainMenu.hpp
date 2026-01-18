#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "ElementGUI/Button.hpp"
#include <filesystem>
#include "Animation/Animation.hpp"

class OptionBox : public ElementGUI {
public:
	sf::RectangleShape _rect;
	std::unique_ptr<sf::Text> _text;
	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	OptionBox(std::wstring text);
	~OptionBox();

	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

class MenuBox : public ElementGUI {
public:
	sf::RectangleShape _rect;
	std::unique_ptr<sf::Text> _text;
	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;
	std::vector<std::shared_ptr<OptionBox>> _options;
	bool _isOpen;

	MenuBox(std::wstring text);
	~MenuBox();

	void addOption(std::shared_ptr<OptionBox> option);
	void setPosition(sf::Vector2i position);
	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

enum class MainMenuStates { Opened, Closing, Closed };

class MainMenu : public ElementGUI {
public:

	sf::RectangleShape _rect;
	std::shared_ptr<sf::Sprite> _logo;
	std::vector<std::shared_ptr<MenuBox>> _menu_boxes;

	MainMenuStates _state;
	std::shared_ptr<MenuBox> _open_menu_box;

	MainMenu();
	~MainMenu();

	sf::Vector2f getSize();
	void setPosition(sf::Vector2i position);
	void saveProject(const std::filesystem::path& path);
	void loadProject(const std::filesystem::path& path);
	void exportAsFile(const std::filesystem::path& path);
	void importAnimation(std::vector<std::shared_ptr<Animation>> newAnimations);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

extern std::shared_ptr<MainMenu> main_menu;
