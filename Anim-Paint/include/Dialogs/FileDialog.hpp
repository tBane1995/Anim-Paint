#pragma once
#include "Dialog.hpp"
#include <filesystem>
#include "ElementGUI/Scrollbar.hpp"
#include "ElementGUI/TextInput.hpp"

std::filesystem::path resolve_lnk(const std::filesystem::path& lnkPath);
bool isExtension(const std::filesystem::path& p, std::wstring extension);
std::filesystem::path getPath(std::filesystem::path p);
std::wstring to_lower_w(std::wstring s);
bool sortkey(std::filesystem::path a, std::filesystem::path b);
bool onlyWhitespace(const std::wstring& s);
bool isDrive(const std::wstring& path);
bool hasChildren(std::filesystem::path& p);

class LocationRect : public ElementGUI {
public:
	std::filesystem::path _path;
	int _depth;
	sf::IntRect _rect;
	std::unique_ptr < sf::Text > _text;

	bool _isOpen;
	std::vector <LocationRect*> _children;

	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	LocationRect(std::wstring path, int depth);
	~LocationRect();

	void setSize(sf::Vector2i size);
	float getTotalHeight();
	void setPosition(sf::Vector2i position);
	void setText(std::wstring text);
	
	void open(const std::function<void(const std::wstring&)>& onPick);	
	void close();
	void hide();

	void unclick();
	void hover();
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};

enum class LocationAndFilesSeparatorState { Idle, Hover, Moving };

class LocationAndFilesSeparator : public ElementGUI {
public:
	sf::IntRect _rect;
	int _minX;
	int _maxX;

	LocationAndFilesSeparatorState _state;
	sf::Vector2i _offset;
	std::function<void()> _func;

	LocationAndFilesSeparator(int linesCount);
	~LocationAndFilesSeparator();

	sf::Vector2i getSize();
	void setPosition(sf::Vector2i position);
	sf::Vector2i getPosition();
	void setRange(float minX, float maxX);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

class FileRect : public ElementGUI {
public:
	std::filesystem::path _path;
	sf::IntRect _rect;
	std::unique_ptr < sf::Text > _text;
	std::wstring _textStr;
	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	FileRect();
	~FileRect();

	void setSize(sf::Vector2i size);
	void setPosition(sf::Vector2i position);
	void setFile(std::filesystem::path path);
	void setText(std::wstring text);
	
	void unclick();
	void hover();
	void click();
	
	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

class FileDialog : public Dialog {
public:

	std::shared_ptr<sf::IntRect> _leftRect;
	std::shared_ptr<sf::IntRect> _rightRect;

	std::vector <LocationRect*> _locations;
	Scrollbar* _leftScrollbar;

	LocationAndFilesSeparator* _separator;

	std::vector <FileRect*> _files;
	Scrollbar* _rightScrollbar;

	std::vector <std::filesystem::path> _filesPaths;
	std::wstring currentPath;

	sf::IntRect _bottomRect;
	sf::Text _filenameText;
	TextInput* _filenameInput;

	ColoredButtonWithText* _selectBtn;
	ColoredButtonWithText* _cancelBtn;

	FileDialog(std::wstring dialogName, std::wstring selectButtonText);
	~FileDialog();

	float calculateLeftScrollbarHeight();

	void createLeftPanel(int linesCount);
	void createSeparator(int linesCount);
	void createRightPanel(int linesCount);

	void loadDirectory();
	void setTheFiles();

	void setPosition(sf::Vector2i position);
	void drawLeftPanel();
	void drawRightPanel();
	void drawBottomPanel();

	void cursorHoverLocations(LocationRect* location);
	void handleEventLocations(LocationRect* location, const sf::Event& event);
	void updateLocations(LocationRect* location);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};