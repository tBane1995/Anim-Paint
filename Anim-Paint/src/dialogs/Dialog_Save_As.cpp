#include "Dialogs/Dialog_Save_As.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Mouse.hpp"
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <cstdio>


FileRect::FileRect() : ElementGUI() {
	_rect = sf::RectangleShape();
	_rect.setFillColor(sf::Color(95, 47, 47));
	_rect.setOutlineThickness(1.0f);
	_rect.setOutlineColor(sf::Color(63, 47, 47));

	_ico = sf::Sprite();

	_text = sf::Text();
	_text.setFont(basicFont);
	_text.setCharacterSize(normal_text_font_size);
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
	_text.setPosition(position + sf::Vector2f(30+4, (30 - basicFont.getLineSpacing(normal_text_font_size))/2.0f));
	
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

	createFileRects(5);
	loadDirectory();
	setPosition(_position);
}

Dialog_Save_As::~Dialog_Save_As() {
	
}

void Dialog_Save_As::createFileRects(int filesCount) {
	for (int i = 0; i < filesCount; i++) {
		FileRect* file = new FileRect();
		file->setSize(sf::Vector2f(360, 30));
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

	// set the names for the file rects
	int scrollbarValue = 5;
	for (int i = 0; i < _files.size(); i++) {
		if (i < _paths.size()) {
			std::wstring filename = _paths[i + scrollbarValue].path().filename().wstring();
			_files[i]->_text.setString(filename);
			_files[i]->_ico.setTexture((_paths[i].is_directory()) ? *getTexture(L"tex\\dialog\\dictionary.png")->_texture : *getTexture(L"tex\\dialog\\file.png")->_texture);
			_files[i]->_path = _paths[i];
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
		_files[i]->setPosition(sf::Vector2f(_position.x + dialog_padding, _position.y + dialog_title_rect_height + dialog_padding + i * 30 + dialog_padding));
	}
}


void Dialog_Save_As::cursorHover() {
	Dialog::cursorHover();
	
	for(auto* file : _files)
		file->cursorHover();
}

void Dialog_Save_As::handleEvent(sf::Event& event) {
	Dialog::handleEvent(event);
}

void Dialog_Save_As::update() {
	Dialog::update();

	for (auto* file : _files)
		file->update();

}

void Dialog_Save_As::draw() {
	Dialog::draw();

	for(auto* file : _files)
		file->draw();
}