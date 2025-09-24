#pragma once
#include "Dialog.hpp"
#include <filesystem>

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

	std::vector <FileRect*> _files;
	std::wstring currentPath;
	std::vector <std::filesystem::directory_entry> _paths;

	Dialog_Save_As();
	~Dialog_Save_As();

	void createFileRects(int filesCount);
	void loadDirectory();
	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};