#pragma once
#include "Dialog.hpp"
#include <filesystem>
#include "ElementGUI/Scrollbar.hpp"

bool sortkey(std::filesystem::path a, std::filesystem::path b);

class LocationRect : public ElementGUI {
public:
	std::filesystem::path _path;
	sf::RectangleShape _rect;
	sf::Sprite _ico;
	sf::Text _text;

	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	LocationRect(std::wstring path);
	~LocationRect();

	void setSize(sf::Vector2f size);
	void setPosition(sf::Vector2f position);
	void setText(std::wstring text);

	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};

enum class LocationAndFilesSeparatorState { Idle, Hover, Moving };

class LocationAndFilesSeparator : public ElementGUI {
public:
	sf::RectangleShape _rect;

	sf::Vector2f _position;
	float _minX;
	float _maxX;

	LocationAndFilesSeparatorState _state;
	sf::Vector2f _offset;
	std::function<void()> _func;

	LocationAndFilesSeparator(int linesCount);
	~LocationAndFilesSeparator();

	void setSize(sf::Vector2f size);
	sf::Vector2f getSize();
	void setPosition(sf::Vector2f position);
	sf::Vector2f getPosition();
	void setRange(float minX, float maxX);

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

	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	FileRect();
	~FileRect();

	void setSize(sf::Vector2f size);
	void setPosition(sf::Vector2f position);
	void setPath(std::filesystem::path path);
	void setText(std::wstring text);
	
	void unclick();
	void hover();
	void click();
	
	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};




class Dialog_Save_As : public Dialog {
public:

	float leftPanelWidth;
	std::vector <LocationRect*> _favorites;
	std::vector <LocationRect*> _libraries;
	std::vector <LocationRect*> _computer;
	Scrollbar* leftScrollbar;

	LocationAndFilesSeparator* separator;

	std::vector <FileRect*> _files;
	Scrollbar* rightScrollbar;

	std::wstring currentPath;
	std::vector <std::filesystem::path> _paths;

	
	Dialog_Save_As();
	~Dialog_Save_As();

	void createLeftPanel(int linesCount);
	void createSeparator(int linesCount);
	void createRightPanel(int linesCount);
	void loadDirectory();
	void sortTheFiles();
	void setTheFiles();
	void setPosition(sf::Vector2f position);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};