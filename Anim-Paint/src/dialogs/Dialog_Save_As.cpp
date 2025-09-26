#include "Dialogs/Dialog_Save_As.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Mouse.hpp"
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <cstdio>

bool sortkey(std::filesystem::directory_entry first, std::filesystem::directory_entry second) {

	if (first.is_directory() && second.is_directory()) {
		std::string name_1 = first.path().filename().string();
		std::string name_2 = second.path().filename().string();

		std::transform(name_1.begin(), name_1.end(), name_1.begin(), [](unsigned char c) { return std::tolower(c); });
		std::transform(name_2.begin(), name_2.end(), name_2.begin(), [](unsigned char c) { return std::tolower(c); });

		if (name_1 < name_2)
			return true;
		else
			return false;
	}
	else if (first.is_directory() && !second.is_directory()) {
		return true;
	}
	else if (!first.is_directory() && second.is_directory()) {
		return false;
	}
	else {
		// both are file
		std::string name_1 = first.path().filename().string();
		std::string name_2 = second.path().filename().string();

		std::transform(name_1.begin(), name_1.end(), name_1.begin(), [](unsigned char c) { return std::tolower(c); });
		std::transform(name_2.begin(), name_2.end(), name_2.begin(), [](unsigned char c) { return std::tolower(c); });

		if (name_1 < name_2)
			return true;
		else
			return false;
	}
}

//////////////////////////////////////////////////////////////////////////////

LocationRect::LocationRect(std::wstring path) : ElementGUI() {
	_rect = sf::RectangleShape();
	_rect.setFillColor(sf::Color(95, 47, 47));
	_rect.setOutlineThickness(1.0f);
	_rect.setOutlineColor(sf::Color(63, 47, 47));

	std::filesystem::directory_entry entry(path);
	_path = entry.path();

	_ico = sf::Sprite();
	if(entry.is_block_file())
		_ico.setTexture(*getTexture(L"tex\\dialog\\harddrive.png")->_texture);
	else if(entry.is_directory())
		_ico.setTexture(*getTexture(L"tex\\dialog\\dictionary.png")->_texture);
	else
		_ico.setTexture(*getTexture(L"tex\\dialog\\empty.png")->_texture);

	_text = sf::Text();
	_text.setFont(basicFont);
	_text.setCharacterSize(dialog_content_font_size);
	_text.setFillColor(normal_text_color);
	_text.setString(entry.path().filename().wstring());

	
}

LocationRect::~LocationRect() {

}

void LocationRect::setSize(sf::Vector2f size) {
	_rect.setSize(size);
}

void LocationRect::setPosition(sf::Vector2f position) {
	_rect.setPosition(position);
	_ico.setPosition(position);
	_text.setPosition(position + sf::Vector2f(20 + 4, (20 - basicFont.getLineSpacing(dialog_content_font_size)) / 2.0f));
	std::cout << basicFont.getLineSpacing(dialog_content_font_size) << std::endl;
}

void LocationRect::setText(std::wstring text) {
	
}

void LocationRect::cursorHover() {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void LocationRect::handleEvent(sf::Event& event) {
}

void LocationRect::update() {
	if (ElementGUI_hovered == this) {
		_rect.setFillColor(sf::Color(95, 47, 47));
	}
	else {
		_rect.setFillColor(sf::Color(63, 47, 47));
	}
}

void LocationRect::draw() {
	window->draw(_rect);
	window->draw(_ico);
	window->draw(_text);
}
////////////////////////////////////////////////////////////////////////////

FileRect::FileRect() : ElementGUI() {
	_rect = sf::RectangleShape();
	_rect.setFillColor(sf::Color(95, 47, 47));
	_rect.setOutlineThickness(1.0f);
	_rect.setOutlineColor(sf::Color(63, 47, 47));

	_ico = sf::Sprite();

	_text = sf::Text();
	_text.setFont(basicFont);
	_text.setCharacterSize(dialog_content_font_size);
	_text.setFillColor(normal_text_color);
	_text.setString(L"pathfile/pathfile/file.png");
	
	
}

FileRect::~FileRect() {

}

void FileRect::setSize(sf::Vector2f size) {
	_rect.setSize(size);
}

void FileRect::setPosition(sf::Vector2f position) {
	_rect.setPosition(position);
	_ico.setPosition(position);
	_text.setPosition(position + sf::Vector2f(20+4, (20 - basicFont.getLineSpacing(dialog_content_font_size))/2.0f));
	std::cout << basicFont.getLineSpacing(dialog_content_font_size) << std::endl;
	
}

void FileRect::setText(std::wstring text) {

}

void FileRect::cursorHover() {

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void FileRect::handleEvent(sf::Event& event) {

}

void FileRect::update() {
	if (ElementGUI_hovered == this) {
		_rect.setFillColor(sf::Color(95, 47, 47));
	}
	else {
		_rect.setFillColor(sf::Color(63, 47, 47));
	}
}

void FileRect::draw() {
	window->draw(_rect);
	window->draw(_ico);
	window->draw(_text);
	
}

//////////////////////////////////////////////////////////////////////////////

Dialog_Save_As::Dialog_Save_As() : Dialog(L"Save As", sf::Vector2f(400, 240), sf::Vector2f(8, 120)) {

	_files.clear();
	
	// current path = desktop
	const wchar_t* userProfile = _wgetenv(L"USERPROFILE");

	if (userProfile) {
		currentPath = userProfile;
		currentPath += L"\\Desktop";
	}
	
	std::wcout << L"Current path: " << currentPath << std::endl;
	createFavorites();
	createFileRects(10);
	loadDirectory();
	sf::Vector2f scrollbarPos = sf::Vector2f(_position.x + 400 - 16 - dialog_padding, _position.y + dialog_title_rect_height + dialog_padding);
	sf::Vector2f scrollbarSize = sf::Vector2f(16, 240 - dialog_title_rect_height - dialog_padding * 2);
	scrollbar = new Scrollbar(scrollbarPos.x, scrollbarPos.y, scrollbarSize.x, scrollbarSize.y, 0, _paths.size() - _files.size() , _files.size(), 0);
	scrollbar->_func = [this]() { setTheFiles(); };
	sortTheFiles();
	setTheFiles();
	setPosition(_position);
}

Dialog_Save_As::~Dialog_Save_As() {
	
}

void Dialog_Save_As::createFavorites() {

	const wchar_t* userProfile = _wgetenv(L"USERPROFILE");
	std::wstring up(userProfile);

	_favorites.push_back(new LocationRect(up + L"\\Desktop"));
	_favorites.push_back(new LocationRect(up + L"\\Downloads"));
	_favorites.push_back(new LocationRect(up + L"\\Recent"));

	// for the test - set the location
	//currentPath = _favorites[2]->_path.wstring(); // error

}

void Dialog_Save_As::createFileRects(int filesCount) {
	for (int i = 0; i < filesCount; i++) {
		FileRect* file = new FileRect();
		file->setSize(sf::Vector2f(360, 20));
		_files.push_back(file);
	}
}

void Dialog_Save_As::loadDirectory() {

	_paths.clear();

	// load paths
	for (const auto& path : std::filesystem::directory_iterator(currentPath)) {
		_paths.push_back(path);
		std::wcout << path.path().wstring() << std::endl;
	}
}

void Dialog_Save_As::sortTheFiles() {
	std::sort(_paths.begin(), _paths.end(), sortkey);
}

void Dialog_Save_As::setTheFiles() {

	// set the names for the file rects
	int scrollbarValue = scrollbar->getValue();
	for (int i = 0; i < _files.size(); i++) {
		if (i + scrollbarValue < _paths.size()) {
			std::wstring filename = _paths[i + scrollbarValue].path().filename().wstring();
			_files[i]->_text.setString(filename);
			_files[i]->_ico.setTexture((_paths[i + scrollbarValue].is_directory()) ? *getTexture(L"tex\\dialog\\dictionary.png")->_texture : *getTexture(L"tex\\dialog\\file.png")->_texture);
			_files[i]->_path = _paths[i + scrollbarValue];
		}
		else {
			_files[i]->_text.setString(L"");
			_files[i]->_path = std::filesystem::path();
			_files[i]->_ico.setTexture(*getTexture(L"tex\\dialog\\empty.png")->_texture);
		}
	}

	// load the harddrivers
	DWORD drives = GetLogicalDrives();
	for (int i = 0; i < 32; i++)
		if ((drives >> i) & 1)
			printf("%c:\\\n", 'A' + i);

}

void Dialog_Save_As::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);

	for(int i = 0; i < _files.size(); i++) {
		_files[i]->setPosition(sf::Vector2f(_position.x + dialog_padding, _position.y + dialog_title_rect_height + dialog_padding + i * 20 ));
	}

	sf::Vector2f scrollbarPos(_position.x + 400 - 16 - dialog_padding, _position.y + dialog_title_rect_height + dialog_padding);
	scrollbar->setPosition(scrollbarPos.x, scrollbarPos.y);
}


void Dialog_Save_As::cursorHover() {
	Dialog::cursorHover();

	if(scrollbar->_state == ScrollbarState::Idle)
		for (auto* file : _files)
			file->cursorHover();

	scrollbar->cursorHover();

}

void Dialog_Save_As::handleEvent(sf::Event& event) {
	Dialog::handleEvent(event);
	scrollbar->handleEvent(event);
}

void Dialog_Save_As::update() {
	Dialog::update();

	for (auto* file : _files)
		file->update();

	scrollbar->update();
}

void Dialog_Save_As::draw() {
	Dialog::draw();

	for(auto* file : _files)
		file->draw();

	scrollbar->draw();
}