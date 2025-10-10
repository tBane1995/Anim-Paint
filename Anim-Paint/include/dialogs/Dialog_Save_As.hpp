#pragma once
#include "Dialog.hpp"
#include <filesystem>
#include "ElementGUI/Scrollbar.hpp"

bool sortkey(std::filesystem::path a, std::filesystem::path b);

class LocationRect : public ElementGUI {
public:
	std::filesystem::path _path;
	int _depth;
	sf::RectangleShape _rect;
	sf::Sprite _arrow;
	sf::Sprite _ico;
	sf::Text _text;

	bool _isOpen;
	std::vector <LocationRect*> _children;

	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	LocationRect(std::wstring path, int depth);
	~LocationRect();

	void setSize(sf::Vector2f size);
	float getTotalHeight();
	void setPosition(sf::Vector2f position);
	void setText(std::wstring text);
	
	void open(const std::function<void(const std::wstring&)>& onPick);	
	void close();
	void hide();

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
	void setFile(std::filesystem::path path);
	void setText(std::wstring text);
	
	void unclick();
	void hover();
	void click();
	
	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

class SelectedFileNameBox : public Button {
public:

	sf::Text _text;
	sf::Text _filename;

	SelectedFileNameBox(sf::Vector2f dialog_size);
	~SelectedFileNameBox();

	void setFilename(std::wstring text);
	void setPosition(sf::Vector2f position);
	sf::Vector2f getPosition();
	sf::Vector2f getSize();

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};


class Dialog_Save_As : public Dialog {
public:

	sf::RectangleShape _leftRect;
	sf::RectangleShape _rightRect;
	
	std::vector <LocationRect*> _locations;
	Scrollbar* _leftScrollbar;

	LocationAndFilesSeparator* _separator;

	std::vector <FileRect*> _files;
	Scrollbar* _rightScrollbar;

	std::vector <std::filesystem::path> _filesPaths;
	std::wstring currentPath;

	sf::RectangleShape _bottomRect;
	sf::Text _filenameText;
	SelectedFileNameBox* _selectedFileNameBox;

	NormalButtonWithText* _selectBtn;
	NormalButtonWithText* _cancelBtn;

	Dialog_Save_As(std::wstring selectButtonText, std::function<void()> function);
	~Dialog_Save_As();

	float calculateLeftScrollbarHeight();

	void createLeftPanel(int linesCount);
	void createSeparator(int linesCount);
	void createRightPanel(int linesCount);

	void addChildren(std::wstring path, int depth);
	void getLocations();
	void setTheLocations();
	void loadDirectory();
	void setTheFiles();

	void setPosition(sf::Vector2f position);
	void drawLeftPanel();
	void drawRightPanel();

	void cursorHoverLocations(LocationRect* location);
	void handleEventLocations(LocationRect* location, sf::Event& event);
	void updateLocations(LocationRect* location);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();

};