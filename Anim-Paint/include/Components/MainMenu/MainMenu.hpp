#pragma once
#include "Element.hpp"
#include "Components/MainMenu/MenuButton.hpp"
#include <filesystem>
#include "Animation/Animation.hpp"
#include "Controls/OptionWithIcon.hpp"

enum class MainMenuStates { Opened, Closing, Closed };

class MainMenu : public Element {
public:

	sf::IntRect _rect;
	std::vector<std::shared_ptr<MenuButton>> _menu_boxes;

	MainMenuStates _state;
	std::shared_ptr<MenuButton> _open_menu_button;

	// MAIN MENU BUTTONS
	std::shared_ptr<MenuButton> file;
	std::shared_ptr<MenuButton> edit;
	std::shared_ptr<MenuButton> tools;
	std::shared_ptr<MenuButton> select;

	// FILE
	std::shared_ptr<OptionWithIcon> file_new;
	std::shared_ptr<OptionWithIcon> file_saveAs;
	std::shared_ptr<OptionWithIcon> file_load;
	std::shared_ptr<OptionWithIcon> file_export;
	std::shared_ptr<OptionWithIcon> file_import;

	// EDIT
	std::shared_ptr<OptionWithIcon> edit_undo;
	std::shared_ptr<OptionWithIcon> edit_redo;

	// TOOLS
	std::shared_ptr<Option> tools_resize;
	std::shared_ptr<Option> tools_rotation;
	std::shared_ptr<Option> tools_brightness_contrast;
	std::shared_ptr<Option> tools_hue_saturation;
	std::shared_ptr<Option> tools_sepia;
	std::shared_ptr<Option> tools_outline;
	std::shared_ptr<Option> tools_invert;
	std::shared_ptr<Option> tools_chessboard;
	std::shared_ptr<Option> tools_smoothing;

	// SELECT
	std::shared_ptr<Option> select_all;
	std::shared_ptr<Option> select_none;
	std::shared_ptr<Option> select_align_center;

	MainMenu();
	~MainMenu();

	sf::Vector2i getSize();
	void resize();
	void setPosition(sf::Vector2i position);
	void hideMenu();	// hide menu - start closing animation - used when opening another menu
	void closeMenu();	// immediately close menu - used when opening dialogs
	void openMenuButton(std::shared_ptr<MenuButton> menuBox);
	void saveProject(const std::filesystem::path& path);
	void loadProject(const std::filesystem::path& path);
	void exportAsFile(const std::filesystem::path& path);
	void importAnimation(std::vector<std::shared_ptr<Animation>> newAnimations);
	bool cursorOnAnyMenuButton();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

extern std::shared_ptr<MainMenu> main_menu;
