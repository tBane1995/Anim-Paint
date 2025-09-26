#include "Dialogs/Dialog_Save_As.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Mouse.hpp"
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <cstdio>
#include <filesystem>
#include <shobjidl.h>   // IShellLink
#include <objbase.h>    // CoInitializeEx
#include <cwctype>   // iswspace
#include "Time.hpp"

inline std::filesystem::path resolve_lnk(const std::filesystem::path& lnkPath) {
	std::filesystem::path result;

	// COINIT_APARTMENTTHREADED jest OK do prostych wywołań Shell
	HRESULT hrCo = CoInitializeEx(nullptr, COINIT_APARTMENTTHREADED);
	bool comInit = SUCCEEDED(hrCo);

	IShellLinkW* psl = nullptr;
	if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER,
		IID_PPV_ARGS(&psl)))) {
		IPersistFile* ppf = nullptr;
		if (SUCCEEDED(psl->QueryInterface(IID_PPV_ARGS(&ppf)))) {
			if (SUCCEEDED(ppf->Load(lnkPath.c_str(), STGM_READ))) {
				WCHAR szTarget[MAX_PATH] = {};
				WIN32_FIND_DATAW wfd{};
				// SLGP_UNCPRIORITY -> preferuj ścieżkę UNC, ale SLGP_RAWPATH też jest ok
				if (SUCCEEDED(psl->GetPath(szTarget, MAX_PATH, &wfd, SLGP_UNCPRIORITY))) {
					result = szTarget;
				}
			}
			ppf->Release();
		}
		psl->Release();
	}

	if (comInit) CoUninitialize();
	return result; // może być pusty, jeśli nie udało się rozwiązać
}


inline bool isExtension(const std::filesystem::path& p, std::wstring extension) {
	std::wstring ext = p.extension().wstring();
	std::transform(ext.begin(), ext.end(), ext.begin(), ::towlower);
	return ext == extension;
}

std::filesystem::path getPath(std::filesystem::path p) {
	if (isExtension(p, L".lnk")) {
		return resolve_lnk(p); // może zwrócić pustą ścieżkę
	}
	return p;
}

static inline std::wstring to_lower_w(std::wstring s) {
	std::transform(s.begin(), s.end(), s.begin(),
		[](wchar_t c) { return static_cast<wchar_t>(std::towlower(c)); });
	return s;
}

bool sortkey(std::filesystem::path a, std::filesystem::path b)
{
	std::error_code ea, eb;
	bool ad = std::filesystem::is_directory(a, ea);
	bool bd = std::filesystem::is_directory(b, eb);
	if (ea) ad = false; // w razie błędu traktuj jak plik
	if (eb) bd = false;

	// katalogi najpierw
	if (ad != bd) return ad;

	// porównanie nazw case-insensitive (na wstring!)
	std::wstring na = to_lower_w(a.filename().wstring());
	std::wstring nb = to_lower_w(b.filename().wstring());
	if (na != nb) return na < nb;

	// tie-breaker żeby utrzymać ścisłe porządki (ważne dla std::sort)
	return a.native() < b.native();
}

bool onlyWhitespace(const std::wstring& s) {
	return std::all_of(s.begin(), s.end(),
		[](wchar_t ch) { return std::iswspace(ch) != 0; });
}

bool isDrive(const std::wstring& path) {
	UINT type = GetDriveTypeW(path.c_str());
	return type != DRIVE_NO_ROOT_DIR && type != DRIVE_UNKNOWN;
}

//////////////////////////////////////////////////////////////////////////////
LocationRect::LocationRect(std::wstring path) : ElementGUI() {
	_rect = sf::RectangleShape();
	_rect.setFillColor(sf::Color(95, 47, 47));
	_rect.setOutlineThickness(1.0f);
	_rect.setOutlineColor(sf::Color(63, 47, 47));

	_path = getPath(path);

	_ico = sf::Sprite();
	if(isDrive(_path))
		_ico.setTexture(*getTexture(L"tex\\dialog\\harddrive.png")->_texture);
	else if(std::filesystem::is_directory(path))
		_ico.setTexture(*getTexture(L"tex\\dialog\\dictionary.png")->_texture);
	else
		_ico.setTexture(*getTexture(L"tex\\dialog\\empty.png")->_texture);

	_text = sf::Text();
	_text.setFont(basicFont);
	_text.setCharacterSize(dialog_content_font_size);
	_text.setFillColor(normal_text_color);

	_text.setString((isDrive(_path)? _path.wstring() : _path.filename().wstring()));
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
	_text.setString(L"...");

	_state = ButtonState::Idle;
	_clickTime = currentTime;
	_onclick_func = []() {};
	
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
	
}

void FileRect::setText(std::wstring text) {
	_text.setString(text);
}

void FileRect::unclick() {
	_state = ButtonState::Idle;
	_rect.setFillColor(sf::Color(63, 47, 47));
}

void FileRect::hover() {
	_state = ButtonState::Hover;
	_rect.setFillColor(sf::Color(95, 47, 47));
}

void FileRect::click() {
	_state = ButtonState::Pressed;
	_rect.setFillColor(sf::Color(79, 47, 47));
	_clickTime = currentTime;
}


void FileRect::cursorHover() {

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void FileRect::handleEvent(sf::Event& event) {

	

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			ElementGUI_pressed = this;
			
		}
		else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
		}

	}
}

void FileRect::update() {
	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();
}

void FileRect::draw() {
	window->draw(_rect);
	window->draw(_ico);
	window->draw(_text);
	
}

//////////////////////////////////////////////////////////////////////////////

Dialog_Save_As::Dialog_Save_As() : Dialog(L"Save As", sf::Vector2f(400, 248), sf::Vector2f(8, 120)) {

	_files.clear();
	
	// current path = desktop
	const wchar_t* userProfile = _wgetenv(L"USERPROFILE");

	if (userProfile) {
		currentPath = userProfile;
		currentPath += L"\\Desktop";
	}
	
	//std::wcout << L"Current path: " << currentPath << std::endl;
	createLeftPanel(10);
	createRightPanel(10);
	loadDirectory();
	
	setTheFiles();
	setPosition(_position);
}

Dialog_Save_As::~Dialog_Save_As() {
	
}

void Dialog_Save_As::createLeftPanel(int dictionariesCount) {
	const wchar_t* userProfile = _wgetenv(L"USERPROFILE");
	std::wstring up(userProfile);

	// dictionaries
	_favorites.push_back(new LocationRect(up + L"\\AppData\\Roaming\\Microsoft\\Windows\\Recent"));
	_favorites.push_back(new LocationRect(up + L"\\Documents"));
	_favorites.push_back(new LocationRect(up + L"\\Music"));
	_favorites.push_back(new LocationRect(up + L"\\Pictures"));
	_favorites.push_back(new LocationRect(up + L"\\Downloads"));
	_favorites.push_back(new LocationRect(up + L"\\Desktop"));
	_favorites.push_back(new LocationRect(up + L"\\Videos"));

	// load the harddrivers
	DWORD drives = GetLogicalDrives();
	for (int i = 0; i < 32; i++)
		if ((drives >> i) & 1) {
			_favorites.push_back(new LocationRect(std::wstring(1, L'A' + i) + L":\\"));
			//printf("%c:\\\n", 'A' + i);
		}
			
	

	for (int i = 0; i < _favorites.size(); i++) {
		std::wcout << _favorites[i]->_path.wstring() << std::endl;
		_favorites[i]->setSize(sf::Vector2f(120, 20));
	}

	// scrollbar
	sf::Vector2f scrollbarPos = sf::Vector2f(_position.x + 136 - 16 - dialog_padding, _position.y + dialog_title_rect_height + dialog_padding);
	sf::Vector2f scrollbarSize = sf::Vector2f(16, 248 - dialog_title_rect_height - dialog_padding * 2);
	leftScrollbar = new Scrollbar(scrollbarPos.x, scrollbarPos.y, scrollbarSize.x, scrollbarSize.y, 0, _favorites.size() - dictionariesCount, dictionariesCount, 0);
	leftScrollbar->_func = [this]() { setTheFiles(); };
}

void Dialog_Save_As::createRightPanel(int filesCount) {
	
	// files
	for (int i = 0; i < filesCount; i++) {
		FileRect* file = new FileRect();
		file->setSize(sf::Vector2f(360-128, 20));
		_files.push_back(file);
	}

	// scrollbar
	sf::Vector2f scrollbarPos = sf::Vector2f(_position.x + 400 - 16 - dialog_padding, _position.y + dialog_title_rect_height + dialog_padding);
	sf::Vector2f scrollbarSize = sf::Vector2f(16, 248 - dialog_title_rect_height - dialog_padding * 2);
	rightScrollbar = new Scrollbar(scrollbarPos.x, scrollbarPos.y, scrollbarSize.x, scrollbarSize.y, 0, _paths.size() - _files.size(), _files.size(), 0);
	rightScrollbar->_func = [this]() { setTheFiles(); };
}

void Dialog_Save_As::sortTheFiles() {
	std::sort(_paths.begin(), _paths.end(), sortkey);
}

void Dialog_Save_As::loadDirectory() {
	//std::wcout << L"Loading directory: " << currentPath << std::endl;
	_paths.clear();

	std::error_code ec;
	auto opts = std::filesystem::directory_options::skip_permission_denied;

	for (std::filesystem::directory_iterator it(currentPath, opts, ec), end;
		it != end; it.increment(ec)) {
		if (ec) { ec.clear(); continue; }

		std::filesystem::path p = getPath(it->path());
		if (p.empty()) continue; // nieudany .lnk

		auto name = p.filename().wstring();
		if (name.empty() || onlyWhitespace(name) || name == L"." || name == L"..")
			continue;

		_paths.push_back(p);
		//std::wcout << p.wstring() << std::endl;
	}

	sortTheFiles();

	if(!isDrive(currentPath))
		_paths.insert(_paths.begin(), std::filesystem::path(currentPath).parent_path());

	//std::wcout << L"Total files: " << _paths.size() << std::endl;
	
}


void Dialog_Save_As::setTheFiles() {

	// set the names for the file rects
	int scrollbarValue = rightScrollbar->getValue();
	for (int i = 0; i < _files.size(); i++) {
		if (i + scrollbarValue < _paths.size()) {

			std::filesystem::path path = _paths[i + scrollbarValue];

			if(i + scrollbarValue == 0 && !isDrive(currentPath))
				_files[i]->_text.setString(L"..");
			else 
				_files[i]->_text.setString(path.filename().wstring());

			std::error_code ec;
			bool isDir = std::filesystem::is_directory(path, ec);
			_files[i]->_ico.setTexture((isDir) ? *getTexture(L"tex\\dialog\\dictionary.png")->_texture : *getTexture(L"tex\\dialog\\file.png")->_texture);
			_files[i]->_path = path;
			
			_files[i]->_onclick_func = [this, path]() {
				if (std::filesystem::is_directory(path)) {
					// is directory then open directory
					std::wcout << L"click\n";
					currentPath = path.wstring();
					loadDirectory();

					this->rightScrollbar->setMax(_paths.size() - _files.size());
					this->rightScrollbar->setValue(0);
					setTheFiles();
				}
				else {
					// is file then open file
					std::wcout << L"Selected file: " << path.wstring() << std::endl;
				}
				};	
			//std::wcout << path.filename().wstring() << std::endl;
		}
		else {
			_files[i]->_text.setString(L"");
			_files[i]->_path = std::filesystem::path();
			_files[i]->_ico.setTexture(*getTexture(L"tex\\dialog\\empty.png")->_texture);
			_files[i]->_onclick_func = []() {};
		}
	}

}

void Dialog_Save_As::setPosition(sf::Vector2f position) {
	Dialog::setPosition(position);

	for(int i = 0; i < _files.size(); i++) {
		_files[i]->setPosition(sf::Vector2f(_position.x + dialog_padding + 128, _position.y + dialog_title_rect_height + dialog_padding + i * 20 ));
	}

	for (int i = 0; i < _favorites.size(); i++) {
		_favorites[i]->setPosition(sf::Vector2f(_position.x + dialog_padding, _position.y + dialog_title_rect_height + dialog_padding + i * 20));
	}

	sf::Vector2f leftScrollbarPos(_position.x + 136 - 16 - dialog_padding, _position.y + dialog_title_rect_height + dialog_padding);
	leftScrollbar->setPosition(leftScrollbarPos.x, leftScrollbarPos.y);

	sf::Vector2f rightScrollbarPos(_position.x + 400 - 16 - dialog_padding, _position.y + dialog_title_rect_height + dialog_padding);
	rightScrollbar->setPosition(rightScrollbarPos.x, rightScrollbarPos.y);

}


void Dialog_Save_As::cursorHover() {
	Dialog::cursorHover();

	leftScrollbar->cursorHover();



	if(rightScrollbar->_state == ScrollbarState::Idle)
		for (auto* file : _files)
			file->cursorHover();

	rightScrollbar->cursorHover();

}

void Dialog_Save_As::handleEvent(sf::Event& event) {
	Dialog::handleEvent(event);

	for(auto& file : _files)
		file->handleEvent(event);

	leftScrollbar->handleEvent(event);
	rightScrollbar->handleEvent(event);
}

void Dialog_Save_As::update() {
	Dialog::update();

	for (auto* file : _files)
		file->update();

	leftScrollbar->update();
	rightScrollbar->update();
}

void Dialog_Save_As::draw() {
	Dialog::draw();

	for(auto* fav : _favorites)
		fav->draw();

	for(auto* file : _files)
		file->draw();

	leftScrollbar->draw();
	rightScrollbar->draw();
}