#pragma once
#include "Dialog.hpp"
#include <filesystem>
#include "ElementGUI/Scrollbar.hpp"

bool sortkey(std::filesystem::directory_entry first, std::filesystem::directory_entry second);

class LocationRect : public ElementGUI {
public:
	std::filesystem::path _path;
	sf::RectangleShape _rect;
	sf::Sprite _ico;
	sf::Text _text;

	LocationRect(std::wstring path);
	~LocationRect();

	void setSize(sf::Vector2f size);
	void setPosition(sf::Vector2f position);
	void setText(std::wstring text);
	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};

class FileRect : public ElementGUI {
public:
	std::filesystem::path _path;
	sf::RectangleShape _rect;
	sf::Sprite _ico;
	sf::Text _text;
	bool _isSelected;

	FileRect();
	~FileRect();

	void setSize(sf::Vector2f size);
	void setPosition(sf::Vector2f position);
	void setText(std::wstring text);
	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};




class Dialog_Save_As : public Dialog {
public:

	
	std::vector <LocationRect*> _favorites;
	std::vector <LocationRect*> _libraries;
	std::vector <LocationRect*> _computer;
	std::vector <FileRect*> _files;

	std::wstring currentPath;
	std::vector <std::filesystem::directory_entry> _paths;

	Scrollbar* scrollbar;

	Dialog_Save_As();
	~Dialog_Save_As();

	void createFavorites();
	void createFileRects(int filesCount);
	void loadDirectory();
	void sortTheFiles();
	void setTheFiles();
	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};