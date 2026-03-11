#pragma once
#include "Dialogs/Dialog.hpp"
#include "Dialogs/FileDialog/LocationRect.hpp"
#include "Dialogs/FileDialog/LocationAndFilesSeparator.hpp"
#include "Dialogs/FileDialog/FileRect.hpp"
#include <filesystem>
#include "Controls/ColoredButtonWithText.hpp"
#include "Controls/Scrollbar.hpp"
#include "Controls/TextInput.hpp"

class FileDialog : public Dialog {
public:

	std::shared_ptr<sf::IntRect> _leftRect;
	std::shared_ptr<sf::IntRect> _rightRect;

	std::vector <std::shared_ptr<LocationRect>> _locations;
	std::shared_ptr<Scrollbar> _leftScrollbar;

	std::shared_ptr<LocationAndFilesSeparator> _separator;

	std::vector <std::shared_ptr<FileRect>> _files;
	std::shared_ptr<Scrollbar> _rightScrollbar;

	std::wstring _acceptableExtension;

	std::vector <std::filesystem::path> _filesPaths;
	std::wstring currentPath;

	sf::IntRect _bottomRect;
	std::unique_ptr<sf::Text> _filenameText;
	int _filenameTextWidth;
	std::shared_ptr<TextInput> _filenameInput;

	std::shared_ptr<ColoredButtonWithText> _selectBtn;
	std::shared_ptr<ColoredButtonWithText> _cancelBtn;

	FileDialog(std::wstring dialogName, std::wstring selectButtonText, std::wstring acceptableExtension = L"");
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

	void cursorHoverLocations(std::shared_ptr<LocationRect> location);
	void handleEventLocations(std::shared_ptr<LocationRect> location, const sf::Event& event);
	void updateLocations(std::shared_ptr<LocationRect> location);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};