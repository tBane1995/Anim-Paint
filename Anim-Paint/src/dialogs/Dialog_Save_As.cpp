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

bool hasChildren(std::filesystem::path& p) {
	std::error_code ec;
	if (!std::filesystem::exists(p, ec) || !std::filesystem::is_directory(p, ec))
		return false;

	auto opts = std::filesystem::directory_options::skip_permission_denied;
	for (std::filesystem::directory_iterator it(p, opts, ec), end; it != end; it.increment(ec)) {

		if (ec) { ec.clear(); continue; }

		if (it->is_directory())
			return true;
	}
	return false;
}

//////////////////////////////////////////////////////////////////////////////
LocationRect::LocationRect(std::wstring path, int depth = 0) : 
	ElementGUI(),
	_arrow(*getTexture(L"tex\\dialog\\dictionaryIsEmpty.png")->_texture),
	_ico(*getTexture(L"tex\\dialog\\empty.png")->_texture),
	_text(basicFont, L"", file_dialog_file_text_size) {

	_rect = sf::RectangleShape();
	unclick();

	_path = getPath(path);
	_depth = depth;

	_isOpen = false;

	if (hasChildren(_path))
		close();
	else
		hide();

	if(isDrive(_path.wstring()))
		_ico.setTexture(*getTexture(L"tex\\dialog\\harddrive.png")->_texture);
	else if(std::filesystem::is_directory(path))
		_ico.setTexture(*getTexture(L"tex\\dialog\\dictionary.png")->_texture);
	else
		_ico.setTexture(*getTexture(L"tex\\dialog\\empty.png")->_texture);

	_text.setFillColor(file_dialog_file_text_color);
	_text.setString((isDrive(_path)? _path.wstring() : _path.filename().wstring()));
}

LocationRect::~LocationRect() {

}

void LocationRect::setSize(sf::Vector2f size) {
	_rect.setSize(size);

	if (_isOpen && !_children.empty()) {
		for(auto& child : _children) {
			child->setSize(size);
		}
	}
}

float LocationRect::getTotalHeight() {
	float h = _rect.getSize().y;

	if (_isOpen) {
		for (auto* child : _children) {
			h += child->getTotalHeight();
		}
	}
	return h;
}

void LocationRect::setPosition(sf::Vector2f position) {

	float arrow_margin = 4;
	float indent = _depth * 12;

	_rect.setPosition(position);
	_arrow.setPosition(position + sf::Vector2f(indent,6));
	_ico.setPosition(position + sf::Vector2f(indent + 8 + arrow_margin,0));
	_text.setPosition(position + sf::Vector2f(indent + 20 + 4 + 8 + arrow_margin, (20 - basicFont.getLineSpacing(dialog_content_font_size)) / 2.0f));

	if (_isOpen && !_children.empty()) {

		float cursorY = position.y + _rect.getSize().y;

		for(auto& child : _children) {
			child->setPosition(sf::Vector2f(position.x, cursorY));
			cursorY += child->getTotalHeight();
		}
	}
}

void LocationRect::setText(std::wstring text) {
	
}

void LocationRect::open(const std::function<void(const std::wstring&)>& onPick)
{
	if (!hasChildren(_path))
		return;

	_isOpen = true;
	_arrow.setTexture(*getTexture(L"tex\\dialog\\dictionaryIsOpened.png")->_texture);

	if (_children.empty()) {

		std::error_code ec;
		auto opts = std::filesystem::directory_options::skip_permission_denied;

		for (std::filesystem::directory_iterator it(_path.wstring(), opts, ec), end;
			it != end; it.increment(ec)) {
			if (ec) { ec.clear(); continue; }

			std::filesystem::path p = getPath(it->path());
			if (p.empty()) continue; // ignore empty and .lnk
			if (!std::filesystem::is_directory(p)) continue; // only directories

			auto name = p.filename().wstring();
			if (name.empty() || onlyWhitespace(name) || name == L"." || name == L"..")
				continue;

			auto* child = new LocationRect(p.wstring(), _depth + 1);
			child->setSize(_rect.getSize());

			child->_onclick_func = [child, onPick]() {

				if (hasChildren(child->_path)) {
					(child->_isOpen) ? child->close() : child->open(onPick);
				}
				if (onPick) onPick(child->_path.wstring());
				};

			_children.push_back(child);
		}
	}

	// jeśli chcesz sortować potomków po nazwie (case-insensitive) — opcjonalnie:
	/*
	auto wlower = [](std::wstring s){
		std::transform(s.begin(), s.end(), s.begin(),
			[](wchar_t c){ return static_cast<wchar_t>(std::towlower(c)); });
		return s;
	};
	std::sort(_children.begin(), _children.end(),
		[&](LocationRect* A, LocationRect* B){
			return wlower(A->_path.filename().wstring()) < wlower(B->_path.filename().wstring());
		});
	*/
}
void LocationRect::close() {
	_isOpen = false;
	_arrow.setTexture(*getTexture(L"tex\\dialog\\dictionaryIsClosed.png")->_texture);
}

void LocationRect::hide() {
	_isOpen = false;
	_arrow.setTexture(*getTexture(L"tex\\dialog\\dictionaryIsEmpty.png")->_texture);
}

void LocationRect::unclick() {
	_state = ButtonState::Idle;
	_rect.setFillColor(file_dialog_file_rect_idle_color);
}

void LocationRect::hover() {
	_state = ButtonState::Hover;
	_rect.setFillColor(file_dialog_file_rect_hover_color);
}

void LocationRect::click() {
	_state = ButtonState::Pressed;
	_rect.setFillColor(file_dialog_file_rect_press_color);
	_clickTime = currentTime;
}


void LocationRect::cursorHover() {
	
	
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	if (_isOpen && !_children.empty()) {
		for (auto& child : _children) {
			child->cursorHover();
		}
	}

}

void LocationRect::handleEvent(const sf::Event& event) {
	
	
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
		}


	}

	if (_isOpen && !_children.empty()) {
		for (auto& child : _children) {
			child->handleEvent(event);
		}
	}

}

void LocationRect::update() {
	

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

	if (_isOpen && !_children.empty()) {
		for (auto& child : _children) {
			child->update();
		}
	}
}

void LocationRect::draw() {
	window->draw(_rect);
	window->draw(_arrow);
	window->draw(_ico);
	window->draw(_text);

	if(_isOpen) {
		for (auto& children : _children) {
			children->draw();
		}
	}

}
////////////////////////////////////////////////////////////////////////////

LocationAndFilesSeparator::LocationAndFilesSeparator(int linesCount) {
	_rect = sf::RectangleShape();
	_rect.setFillColor(file_dialog_separator_idle_color);
	setSize(sf::Vector2f(file_dialog_separator_width, linesCount * file_dialog_file_rect_height));

	_state = LocationAndFilesSeparatorState::Idle;
}

LocationAndFilesSeparator::~LocationAndFilesSeparator() {

}

void LocationAndFilesSeparator::setSize(sf::Vector2f size) {
	_rect.setSize(size);
}

sf::Vector2f LocationAndFilesSeparator::getSize() {
	return _rect.getSize();
}

void LocationAndFilesSeparator::setPosition(sf::Vector2f position) {
	_position = position;
	_rect.setPosition(_position);
}

sf::Vector2f LocationAndFilesSeparator::getPosition() {
	return _rect.getPosition();
}

void LocationAndFilesSeparator::setRange(float minX, float maxX) {
	_minX = minX;
	_maxX = maxX;
}

void LocationAndFilesSeparator::cursorHover() {

	if (_rect.getGlobalBounds().contains(worldMousePosition) || _state == LocationAndFilesSeparatorState::Moving) {
		_rect.setFillColor(file_dialog_separator_hover_color);
		ElementGUI_hovered = this;
	}
	else
		_rect.setFillColor(file_dialog_separator_idle_color);
}

void LocationAndFilesSeparator::handleEvent(const sf::Event& event) {
	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (ElementGUI_pressed == this) {
			ElementGUI_pressed = nullptr;
			_state = LocationAndFilesSeparatorState::Idle;
		}
	}
	else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_pressed = this;
			_state = LocationAndFilesSeparatorState::Moving;
			_offset = worldMousePosition - sf::Vector2f(_rect.getPosition().x, _rect.getPosition().y);

		}
	}
	


}

void LocationAndFilesSeparator::update() {

	if (_state == LocationAndFilesSeparatorState::Moving) {

		sf::Vector2f newPos = worldMousePosition - _offset;
		newPos.x = std::clamp(newPos.x, _minX, _maxX);
		_rect.setFillColor(file_dialog_separator_press_color);
		_rect.setPosition(sf::Vector2f(newPos.x, _rect.getPosition().y));
		if (_func)
			_func();
	}
}

void LocationAndFilesSeparator::draw() {
	window->draw(_rect);
}
////////////////////////////////////////////////////////////////////////////

FileRect::FileRect() : 
	ElementGUI(),
	_ico(*getTexture(L"tex\\dialog\\empty.png")->_texture),
	_text(basicFont, L"...", file_dialog_file_text_size) {

	_rect = sf::RectangleShape();
	_rect.setFillColor(sf::Color(95, 47, 47));

	_text.setFillColor(file_dialog_file_text_color);

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

void FileRect::setFile(std::filesystem::path path) {
	_path = path;

	if (isDrive(_path.wstring()))
		_ico.setTexture(*getTexture(L"tex\\dialog\\harddrive.png")->_texture);
	else if (std::filesystem::is_directory(path))
		_ico.setTexture(*getTexture(L"tex\\dialog\\dictionary.png")->_texture);
	else if (path != std::filesystem::path())
		_ico.setTexture(*getTexture(L"tex\\dialog\\file.png")->_texture);
	else
		_ico.setTexture(*getTexture(L"tex\\dialog\\empty.png")->_texture);
}

void FileRect::setText(std::wstring text) {
	_text.setString(text);
}

void FileRect::unclick() {
	_state = ButtonState::Idle;
	_rect.setFillColor(file_dialog_file_rect_idle_color);
}

void FileRect::hover() {
	_state = ButtonState::Hover;
	_rect.setFillColor(file_dialog_file_rect_hover_color);
}

void FileRect::click() {
	_state = ButtonState::Pressed;
	_rect.setFillColor(file_dialog_file_rect_press_color);
	_clickTime = currentTime;
}


void FileRect::cursorHover() {

	if (_path != std::filesystem::path()) {
		if (_rect.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}
}

void FileRect::handleEvent(const sf::Event& event) {

	
	if (_path != std::filesystem::path()) {
		if (_rect.getGlobalBounds().contains(worldMousePosition)) {
			if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
				ElementGUI_pressed = this;

			}
			else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
				if (ElementGUI_pressed == this) {
					click();
				}
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

SelectedFileNameBox::SelectedFileNameBox(sf::Vector2f size) : 
	Button(),
	_filename(basicFont, L"", file_dialog_file_text_size) {
	
	_rect.setSize(size);
	_rect.setFillColor(sf::Color(79, 79, 79));

	_filename.setFillColor(file_dialog_file_text_color);
}

SelectedFileNameBox::~SelectedFileNameBox() {

}

void SelectedFileNameBox::setFilename(std::wstring text) {
	_filename.setString(text);
}

std::wstring SelectedFileNameBox::getFilename() {
	return _filename.getString();
}

void SelectedFileNameBox::setPosition(sf::Vector2f position) {
	_rect.setPosition(position);
	_filename.setPosition(_rect.getPosition() + sf::Vector2f(2,0));
}

sf::Vector2f SelectedFileNameBox::getPosition() {
	return _rect.getPosition();
}

sf::Vector2f SelectedFileNameBox::getSize() {
	return _rect.getSize();
}

void SelectedFileNameBox::cursorHover() {

}

void SelectedFileNameBox::handleEvent(const sf::Event& event) {

}

void SelectedFileNameBox::update() {

}

void SelectedFileNameBox::draw() {
	window->draw(_rect);

	sf::View view(sf::FloatRect(
		{ _rect.getPosition().x,_rect.getPosition().y }, 
		{ _rect.getSize().x, _rect.getSize().y }
	));

	sf::FloatRect vp(
		{ _rect.getPosition().x / mainView.getSize().x, _rect.getPosition().y / mainView.getSize().y },
		{ _rect.getSize().x / mainView.getSize().x, _rect.getSize().y / mainView.getSize().y }
	);

	view.setViewport(vp);

	window->setView(view);

	window->draw(_filename);

	window->setView(mainView);
	
}
 
//////////////////////////////////////////////////////////////////////////////

Dialog_Save_As::Dialog_Save_As(std::wstring dialogName, std::wstring selectButtonText, std::function<void(const std::filesystem::path&)> function) : 
	Dialog(dialogName, sf::Vector2f(400, 292), sf::Vector2f(8, 120)),
	_filenameText(basicFont, L"File name", file_dialog_file_text_size)
{

	_files.clear();
	
	// current path = desktop
	const wchar_t* userProfile = _wgetenv(L"USERPROFILE");

	if (userProfile) {
		currentPath = userProfile;
		currentPath += L"\\Desktop";
	}
	
	//std::wcout << L"Current path: " << currentPath << std::endl;
	createLeftPanel(9);
	createSeparator(9);
	createRightPanel(9);
	loadDirectory();
	
	setTheFiles();

	_bottomRect = sf::RectangleShape(sf::Vector2f(getContentSize().x, 70));
	_bottomRect.setFillColor(sf::Color(47, 47, 47));

	_filenameText.setFillColor(file_dialog_file_text_color);

	_selectedFileNameBox = new SelectedFileNameBox(sf::Vector2f(getContentSize().x - _filenameText.getGlobalBounds().size.x - 3 * dialog_padding, file_dialog_file_rect_height));

	sf::Vector2f btnSize(64, file_dialog_file_rect_height + dialog_padding);
	_selectBtn = new NormalButtonWithText(selectButtonText, btnSize);
	_selectBtn->setColors(sf::Color(31, 31, 31), sf::Color(7,7,7), sf::Color(23, 23, 23));
	_selectBtn->_onclick_func = [this, function]() {
		std::wstring name = _selectedFileNameBox->getFilename();
		if (name.empty() || onlyWhitespace(name)) {
			return;
		}
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;
		if (fullPath.extension().empty()) 
			fullPath.replace_extension(L".png");

		if (function) function(fullPath);

		_state = DialogState::ToClose;

		};

	_cancelBtn = new NormalButtonWithText(L"Cancel", btnSize);
	_cancelBtn->setColors(sf::Color(31, 31, 31), sf::Color(7, 7, 7), sf::Color(23, 23, 23));
	_cancelBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};

	setPosition(_position);
}

Dialog_Save_As::~Dialog_Save_As() {

	for (auto& file : _files) {
		delete file;
	}

	for (auto& loc : _locations) {
		for (auto& child : loc->_children) {
			delete child;
		}

		delete loc;
	}

	delete _selectedFileNameBox;
}

float Dialog_Save_As::calculateLeftScrollbarHeight() {
	float hgh = 0;
	for (auto& fav : _locations)
		hgh += fav->getTotalHeight();
	hgh -= _leftRect.getSize().y;
	return hgh;
}

void Dialog_Save_As::createLeftPanel(int dictionariesCount) {

	_leftRect = sf::RectangleShape(sf::Vector2f(100.0f, (dictionariesCount) * file_dialog_file_rect_height));
	_leftRect.setFillColor(sf::Color(255, 47, 47, 127));

	const wchar_t* userProfile = _wgetenv(L"USERPROFILE");
	std::wstring up(userProfile);

	// dictionaries
	_locations.push_back(new LocationRect(up + L"\\AppData\\Roaming\\Microsoft\\Windows\\Recent"));
	_locations.push_back(new LocationRect(up + L"\\Documents"));
	_locations.push_back(new LocationRect(up + L"\\Music"));
	_locations.push_back(new LocationRect(up + L"\\Pictures"));
	_locations.push_back(new LocationRect(up + L"\\Downloads"));
	_locations.push_back(new LocationRect(up + L"\\Desktop"));
	_locations.push_back(new LocationRect(up + L"\\Videos"));

	// load the harddrivers
	DWORD drives = GetLogicalDrives();
	for (int i = 0; i < 32; i++)
		if ((drives >> i) & 1) {
			//printf("%c:\\\n", 'A' + i);
			_locations.push_back(new LocationRect(std::wstring(1, L'A' + i) + L":\\"));
		}

	for (int i = 0; i < _locations.size(); i++) {
		_locations[i]->setSize(sf::Vector2f(_leftRect.getSize().x, file_dialog_file_rect_height));

		_locations[i]->_onclick_func = [this, i]() {
			// ustaw bieżącą ścieżkę na kliknięty węzeł
			currentPath = _locations[i]->_path.wstring();

			// przełącz rozwinięcie gałęzi; dla POTOMKÓW użyjemy callbacka onPick
			if (_locations[i]->_isOpen) {
				_locations[i]->close();
			}
			else {
				_locations[i]->open([this](const std::wstring& newPath) {
					_leftScrollbar->setMax(calculateLeftScrollbarHeight());
					currentPath = newPath;
					loadDirectory();
					// prawa kolumna
					_rightScrollbar->setMax((_filesPaths.size() - _files.size() + 1) * file_dialog_file_rect_height);
					_rightScrollbar->setValue(0);
					setTheFiles();
					setPosition(_position);
					});
			}

			
			_leftScrollbar->setMax(calculateLeftScrollbarHeight());
			_leftScrollbar->setValue(_leftScrollbar->_value); // value clamp
			// >>> KLUCZ: natychmiastowy refresh po open/close dla WĘZŁA GŁÓWNEGO <<<
			loadDirectory();

			// prawa kolumna
			_rightScrollbar->setMax((_filesPaths.size() - _files.size() + 1) * file_dialog_file_rect_height);
			_rightScrollbar->setValue(0);
			setTheFiles();

			// przelicz pozycje (ustawi pozycje dzieci, bo LocationRect::setPosition to robi gdy _isOpen)
			setPosition(_position);
			};
	}



	// scrollbar
	sf::Vector2f scrollbarPos = { _leftRect.getPosition().x + _leftRect.getSize().x + dialog_padding, getContentSize().y + dialog_padding };
	sf::Vector2f scrollbarSize = { 16, _leftRect.getSize().y };

	float scrollbarMax = calculateLeftScrollbarHeight();
	float scrollbarSliderSize = (dictionariesCount - 1) * file_dialog_file_rect_height;

	_leftScrollbar = new Scrollbar(scrollbarPos.x, scrollbarPos.y, scrollbarSize.x, scrollbarSize.y, 0, scrollbarMax, scrollbarSliderSize, 0);
	_leftScrollbar->_func = [this]() { 
		setTheFiles();
		setPosition(_position);
		};
	_leftScrollbar->setScrollArea(&_leftRect, file_dialog_file_rect_height * 0.5f);
}

void Dialog_Save_As::createSeparator(int linesCount) {
	_separator = new LocationAndFilesSeparator(linesCount);
	_separator->_rect.setPosition(sf::Vector2f(getContentPosition().x + _leftRect.getSize().x + 16 + dialog_padding, getContentPosition().y + dialog_padding));
	_separator->setRange(_separator->_rect.getPosition().x, _separator->_rect.getPosition().x + 128);
	_separator->_func = [this]() {
		_leftRect.setSize(sf::Vector2f(_separator->getPosition().x - _leftRect.getPosition().x - 16, _leftRect.getSize().y));

		for(int i=0;i<_locations.size(); i++)
			_locations[i]->setSize(sf::Vector2f(_leftRect.getSize().x, file_dialog_file_rect_height));

		_rightRect.setSize(sf::Vector2f(getContentSize().x - _leftRect.getSize().x - _separator->getSize().x - 2 * 16 - dialog_padding * 3, _rightRect.getSize().y));
		for (int i = 0; i < _files.size(); i++)
			_files[i]->setSize(sf::Vector2f(_rightRect.getSize().x, file_dialog_file_rect_height));

		setPosition(_position);
		};
}

void Dialog_Save_As::createRightPanel(int linesCount) {
	
	float w = getContentSize().x - _leftRect.getSize().x - 2 * 16 - _separator->getSize().x - 3*dialog_padding;
	_rightRect = sf::RectangleShape(sf::Vector2f(w, linesCount*file_dialog_file_rect_height));
	_rightRect.setFillColor(sf::Color(255, 47, 47, 127));

	// files
	for (int i = 0; i < linesCount+1; i++) {
		FileRect* file = new FileRect();
		file->setSize(sf::Vector2f(_rightRect.getSize().x, file_dialog_file_rect_height));
		_files.push_back(file);
	}

	// scrollbar
	sf::Vector2f scrollbarPos = sf::Vector2f(_position.x + getContentSize().x - 16 - dialog_padding, _position.y + dialog_title_rect_height + dialog_padding);
	sf::Vector2f scrollbarSize = sf::Vector2f(16, _rightRect.getSize().y);
	float minValue = 0;
	float maxValue = (_filesPaths.size() - _files.size()) * file_dialog_file_rect_height;
	float sliderSize = (_files.size() - 1) * file_dialog_file_rect_height;

	_rightScrollbar = new Scrollbar(scrollbarPos.x, scrollbarPos.y, scrollbarSize.x, scrollbarSize.y, minValue, maxValue, sliderSize, 0);
	_rightScrollbar->_func = [this]() { 
		setTheFiles();
		setPosition(_position);
		};
	_rightScrollbar->setScrollArea(&_rightRect, file_dialog_file_rect_height * 0.5f);
}


void Dialog_Save_As::loadDirectory() {
	//std::wcout << L"Loading directory: " << currentPath << std::endl;
	_filesPaths.clear();

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

		_filesPaths.push_back(p);
		//std::wcout << p.wstring() << std::endl;
	}

	std::sort(_filesPaths.begin(), _filesPaths.end(), sortkey);

	if(!isDrive(currentPath))
		_filesPaths.insert(_filesPaths.begin(), std::filesystem::path(currentPath).parent_path());

	//std::wcout << L"Total files: " << _paths.size() << std::endl;
	
	_rightScrollbar->setMax((_filesPaths.size() - _files.size()+1)*file_dialog_file_rect_height);
	
}

void Dialog_Save_As::setTheFiles() {

	// set the names for the file rects
	int scrollbarValue = _rightScrollbar->getValue();

	for (int i = 0; i < _files.size(); i++) {
		if (i * file_dialog_file_rect_height + scrollbarValue < _filesPaths.size() * file_dialog_file_rect_height) {

			std::filesystem::path path = _filesPaths[i + scrollbarValue/file_dialog_file_rect_height];

			_files[i]->setFile(path);
			
			if(_files[i]->_path == std::filesystem::path(currentPath).parent_path() && !isDrive(currentPath))
				_files[i]->_text.setString(L"..");
			else 
				_files[i]->_text.setString(path.filename().wstring());
			
			_files[i]->_onclick_func = [this, path]() {
				if (std::filesystem::is_directory(path)) {
					// is directory then open directory
					currentPath = path.wstring();
					loadDirectory();

					_rightScrollbar->setMax((_filesPaths.size() - _files.size()+1)*file_dialog_file_rect_height);
					_rightScrollbar->setValue(0);
					setTheFiles();
					setPosition(_position);

				}
				else {
					// is file then open file
					std::wcout << L"Selected file: " << path.wstring() << std::endl;
					_selectedFileNameBox->setFilename(path.filename().wstring());
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

	// remember old positions for separator
	sf::Vector2f oldPos = getContentPosition();
	float oldAbsRangeMinX = _separator->_minX - getContentPosition().x;
	float oldAbsRangeMaxX = _separator->_maxX - getContentPosition().x;
	sf::Vector2f oldAbsSeparatorPos = _separator->getPosition() - oldPos;

	// set the position
	Dialog::setPosition(position);

	// rects
	_leftRect.setPosition(sf::Vector2f(getContentPosition().x + dialog_padding, getContentPosition().y + dialog_padding));
	_rightRect.setPosition(sf::Vector2f(getContentPosition().x + getContentSize().x - _rightRect.getSize().x - 3 * dialog_padding, getContentPosition().y + dialog_padding));
	
	// left rect
	sf::Vector2f pos;
	pos.x = getContentPosition().x + dialog_padding;
	pos.y = getContentPosition().y + dialog_padding - _leftScrollbar->getValue();
	for (int i = 0; i < _locations.size(); i++) {
		_locations[i]->setPosition(pos);
		pos.y += _locations[i]->getTotalHeight();
	}

	// separator
	_separator->setPosition(getContentPosition() + oldAbsSeparatorPos);
	_separator->setRange(getContentPosition().x + oldAbsRangeMinX, getContentPosition().x + oldAbsRangeMaxX);

	// right rect
	for (int i = 0; i < _files.size(); i++) {
		sf::Vector2f pos;
		pos.x = _separator->getPosition().x + _separator->getSize().x + dialog_padding;
		pos.x = _rightRect.getPosition().x;
		pos.y = getContentPosition().y + dialog_padding + (i * file_dialog_file_rect_height) - _rightScrollbar->getValue() % int(file_dialog_file_rect_height);
		//std::wcout << rightScrollbar->getValue() << " : " << pos.y << L"\n";
		_files[i]->setPosition(pos);
	}

	// left scrollbar
	sf::Vector2f leftScrollbarPos(_leftRect.getPosition().x + _leftRect.getSize().x, _leftRect.getPosition().y);
	_leftScrollbar->setPosition(leftScrollbarPos.x, leftScrollbarPos.y);

	// right scrollbar
	sf::Vector2f rightScrollbarPos(getContentPosition().x + getContentSize().x - 16 - dialog_padding, getContentPosition().y + dialog_padding);
	_rightScrollbar->setPosition(rightScrollbarPos.x, rightScrollbarPos.y);

	// selected file name box
	sf::Vector2f bottomRectPos;
	bottomRectPos.x = getContentPosition().x;
	bottomRectPos.y = std::max(_rightRect.getPosition().y + _rightRect.getSize().y, _leftRect.getPosition().y + _leftRect.getSize().y);
	_bottomRect.setPosition(bottomRectPos);

	_filenameText.setPosition(bottomRectPos + sf::Vector2f(dialog_padding,dialog_padding));

	sf::Vector2f selectedFileNameBoxPos(sf::Vector2f(getContentPosition().x + _filenameText.getGlobalBounds().size.x + 2 * dialog_padding, _rightRect.getPosition().y + _rightRect.getSize().y + dialog_padding));
	_selectedFileNameBox->setPosition(selectedFileNameBoxPos);

	sf::Vector2f btnPos;
	btnPos.x = getContentPosition().x + getContentSize().x - _cancelBtn->getSize().x - dialog_padding;
	btnPos.y = _selectedFileNameBox->getPosition().y + _selectedFileNameBox->getSize().y + dialog_padding;
	_cancelBtn->setPosition(btnPos);

	btnPos.x -= dialog_padding + _selectBtn->getSize().x;
	_selectBtn->setPosition(btnPos);

}

void Dialog_Save_As::drawLeftPanel() {

	sf::View view(sf::FloatRect(
		{ _leftRect.getPosition().x, _leftRect.getPosition().y, },
		{ _leftRect.getSize().x, _leftRect.getSize().y }
	));

	sf::FloatRect vp(
		{ _leftRect.getPosition().x / mainView.getSize().x, _leftRect.getPosition().y / mainView.getSize().y },
		{ _leftRect.getSize().x / mainView.getSize().x, _leftRect.getSize().y / mainView.getSize().y }
	);

	view.setViewport(vp);

	window->setView(view);

	for (auto* fav : _locations)
		fav->draw();

	window->setView(mainView);
}

void Dialog_Save_As::drawRightPanel() {

	sf::View view(sf::FloatRect(
		{ _rightRect.getPosition().x, _rightRect.getPosition().y },
		{ _rightRect.getSize().x, _rightRect.getSize().y }
	));

	sf::FloatRect vp(
		{ _rightRect.getPosition().x / mainView.getSize().x, _rightRect.getPosition().y / mainView.getSize().y },
		{ _rightRect.getSize().x / mainView.getSize().x, _rightRect.getSize().y / mainView.getSize().y }
	);
	view.setViewport(vp);

	window->setView(view);

	for (auto* file : _files)
		file->draw();

	window->setView(mainView);
}

void Dialog_Save_As::cursorHoverLocations(LocationRect* location)
{
	location->cursorHover();

	if (location->_isOpen) {
		for (auto& child : location->_children) {
			cursorHoverLocations(child);
		}
	}
}

void Dialog_Save_As::handleEventLocations(LocationRect* location, const sf::Event& event)
{
	location->handleEvent(event);

	if (location->_isOpen) {
		for (auto& child : location->_children) {
			handleEventLocations(child, event);
		}
	}
}

void Dialog_Save_As::updateLocations(LocationRect* location)
{
	location->update();

	if (location->_isOpen) {
		for (auto& child : location->_children) {
			updateLocations(child);
		}
	}
}

void Dialog_Save_As::cursorHover() {
	Dialog::cursorHover();

	if (_rightScrollbar->_state == ScrollbarState::Idle && _leftScrollbar->_state == ScrollbarState::Idle) {
		if (_rightRect.getGlobalBounds().contains(worldMousePosition)) {
			for (auto* file : _files)
				file->cursorHover();
		}
		
		for (auto* fav : _locations) {
			cursorHoverLocations(fav);
		}
	}
		
	_leftScrollbar->cursorHover();
	_rightScrollbar->cursorHover();

	_separator->cursorHover();

	_selectBtn->cursorHover();
	_cancelBtn->cursorHover();
}

void Dialog_Save_As::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_leftScrollbar->handleEvent(event);
	_rightScrollbar->handleEvent(event);

	if (_rightRect.getGlobalBounds().contains(worldMousePosition)) {
		for (auto& file : _files)
			file->handleEvent(event);
	}

	if (_leftRect.getGlobalBounds().contains(worldMousePosition)) {
		for (auto* fav : _locations) {
			handleEventLocations(fav, event);
		}
	}

	_separator->handleEvent(event);

	_selectBtn->handleEvent(event);
	_cancelBtn->handleEvent(event);
}

void Dialog_Save_As::update() {
	Dialog::update();

	for (auto* file : _files)
		file->update();

	for (auto* fav : _locations) {
		updateLocations(fav);
	}

	_leftScrollbar->update();
	_rightScrollbar->update();
	
	_separator->update();

	_selectBtn->update();
	_cancelBtn->update();
}

void Dialog_Save_As::draw() {
	Dialog::draw();

	drawLeftPanel();
	drawRightPanel();

	_leftScrollbar->draw();
	_rightScrollbar->draw();

	_separator->draw();

	window->draw(_bottomRect);
	window->draw(_filenameText);
	_selectedFileNameBox->draw();

	_selectBtn->draw();
	_cancelBtn->draw();

	//window->draw(_leftRect);
	//window->draw(_rightRect);
}