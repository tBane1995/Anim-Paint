#pragma once
#include "Element.hpp"
#include "Components/MainMenu/MenuBox.hpp"
#include "Components/MainMenu/OptionBox.hpp"
#include <filesystem>
#include "Animation/Animation.hpp"

enum class MainMenuStates { Opened, Closing, Closed };

class MainMenu : public Element {
public:

	sf::IntRect _rect;
	std::vector<std::shared_ptr<MenuBox>> _menu_boxes;

	MainMenuStates _state;
	std::shared_ptr<MenuBox> _open_menu_box;

	MainMenu();
	~MainMenu();

	sf::Vector2i getSize();
	void resize();
	void setPosition(sf::Vector2i position);
	void hideMenu();	// hide menu - start closing animation - used when opening another menu
	void closeMenu();	// immediately close menu - used when opening dialogs
	void openMenuBox(std::shared_ptr<MenuBox> menuBox);
	void saveProject(const std::filesystem::path& path);
	void loadProject(const std::filesystem::path& path);
	void exportAsFile(const std::filesystem::path& path);
	void importAnimation(std::vector<std::shared_ptr<Animation>> newAnimations);
	bool cursorOnAnyMenuBox();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

extern std::shared_ptr<MainMenu> main_menu;
