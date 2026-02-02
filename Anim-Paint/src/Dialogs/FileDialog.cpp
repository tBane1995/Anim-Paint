#include "Dialogs/FileDialog.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Cursor.hpp"
#include <iostream>
#include <cstdlib>
#include <windows.h>
#include <cstdio>
#include <filesystem>
#include <shobjidl.h>   // IShellLink
#include <cwctype>   // iswspace
#include "Time.hpp"

std::filesystem::path resolve_lnk(const std::filesystem::path& lnkPath) {
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


bool isExtension(const std::filesystem::path& p, std::wstring extension) {
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

std::wstring to_lower_w(std::wstring s) {
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
LocationRect::LocationRect(std::wstring path, int depth = 0) : ElementGUI() {

	_rect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(0,0));
	unclick();

	_path = getPath(path);
	_depth = depth;

	_isOpen = false;

	_clickType = LocationRectClickType::None;

	if (hasChildren(_path))
		close();
	else
		hide();
}

LocationRect::~LocationRect() {

}

void LocationRect::setSize(sf::Vector2i size) {
	_rect.size = sf::Vector2i(size);

	if (_isOpen && !_children.empty()) {
		for(auto& child : _children) {
			child->setSize(size);
		}
	}
}

float LocationRect::getTotalHeight() {
	float h = (float)_rect.size.y;

	if (_isOpen) {
		for (auto& child : _children) {
			h += child->getTotalHeight();
		}
	}
	return h;
}

void LocationRect::setPosition(sf::Vector2i position) {


	_rect.position = sf::Vector2i(position);

	if (_isOpen && !_children.empty()) {

		float cursorY = (float)(position.y + _rect.size.y);

		for(auto& child : _children) {
			sf::Vector2i childPosition = sf::Vector2i(position.x, (int)(cursorY));
			child->setPosition(childPosition);
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

			auto child = std::make_shared<LocationRect>(p.wstring(), _depth + 1);
			child->setSize(_rect.size);

			child->_onclick_arrow_func = [child, onPick]() {
				if (!hasChildren(child->_path)) return;
				if (child->_isOpen) {
					child->close();
				}
				else {
					child->open(onPick);
				}
				};

			child->_onclick_location_func = [child, onPick]() {
				
				if (onPick) {
					onPick(child->_path.wstring());
				}
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
}

void LocationRect::hide() {
	_isOpen = false;
}

void LocationRect::unclick() {
	_state = ButtonState::Idle;
	_clickType = LocationRectClickType::None;
}

void LocationRect::hover() {
	_state = ButtonState::Hover;
}

void LocationRect::locationClick() {
	_state = ButtonState::Pressed;
	_clickType = LocationRectClickType::ClickLocation;
	_clickTime = currentTime;
}

void LocationRect::arrowClick() {
	_state = ButtonState::Pressed;
	_clickType = LocationRectClickType::ClickArrow;
	_clickTime = currentTime;
}

void LocationRect::cursorHover() {
	
	
	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}

	if (_isOpen && !_children.empty()) {
		for (auto& child : _children) {
			child->cursorHover();
		}
	}

}

void LocationRect::handleEvent(const sf::Event& event) {
	
	
	if (_rect.contains(cursor->_worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {

			float indent = _rect.position.x + _depth * _indentDelta;

			if ( indent < cursor->_worldMousePosition.x && cursor->_worldMousePosition.x < indent + _arrowMargin + 8) {
				ElementGUI_pressed = this->shared_from_this();
				_clickType = LocationRectClickType::ClickArrow;
			}
			else {
				ElementGUI_pressed = this->shared_from_this();
				_clickType = LocationRectClickType::ClickLocation;
			}
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this->shared_from_this()) {
				if (_clickType == LocationRectClickType::ClickArrow) {
					arrowClick();
				}
				else if(_clickType == LocationRectClickType::ClickLocation) {
					locationClick();
				}
				
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
			if (this->_clickType == LocationRectClickType::ClickArrow) {
				if (_onclick_arrow_func)
					_onclick_arrow_func();
			}

			if (this->_clickType == LocationRectClickType::ClickLocation) {
				if (_onclick_location_func)
					_onclick_location_func();
			}

			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered.get() == this) {
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

	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	switch (_state)
	{
	case ButtonState::Pressed:
		rect.setFillColor(file_dialog_file_rect_press_color);
		break;
	case ButtonState::Hover:
		rect.setFillColor(file_dialog_file_rect_hover_color);
		break;
	case ButtonState::Idle:
		rect.setFillColor(file_dialog_file_rect_idle_color);
		break;
	}

	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);

	float indent = _depth * _indentDelta;

	sf::Sprite arrow(*getTexture(_isOpen ? L"tex\\file_dialog\\dictionaryIsOpened.png" : hasChildren(_path) ? L"tex\\file_dialog\\dictionaryIsClosed.png" : L"tex\\file_dialog\\dictionaryIsEmpty.png")->_texture);
	arrow.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(indent, 6));
	window->draw(arrow);

	sf::Sprite ico(*getTexture(isDrive(_path.wstring()) ? L"tex\\file_dialog\\harddrive.png" : std::filesystem::is_directory(_path) ? L"tex\\file_dialog\\dictionary.png" : L"tex\\file_dialog\\empty.png")->_texture);
	ico.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(indent + 8 + _arrowMargin, 0));
	window->draw(ico);

	if (_text == nullptr) {
		_text = std::make_unique<sf::Text>(basicFont, L"", basic_text_size);
		_text->setFillColor(basic_text_color);
		_text->setString((isDrive(_path) ? _path.wstring() : _path.filename().wstring()));
	}
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(indent + 20 + 4 + 8 + _arrowMargin, (20 - basicFont.getLineSpacing(dialog_content_font_size)) / 2.0f));
	window->draw(*_text);

	if (_isOpen) {
		for (auto& children : _children) {
			children->draw();
		}
	}

}
////////////////////////////////////////////////////////////////////////////

LocationAndFilesSeparator::LocationAndFilesSeparator(int linesCount) : ElementGUI() {
	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(file_dialog_separator_width, linesCount * basic_text_rect_height));
	_minX = _maxX = 0;
	_state = LocationAndFilesSeparatorState::Idle;
}

LocationAndFilesSeparator::~LocationAndFilesSeparator() {

}

sf::Vector2i LocationAndFilesSeparator::getSize() {
	return _rect.size;
}

void LocationAndFilesSeparator::setPosition(sf::Vector2i position) {
	_rect.position = position;
}

sf::Vector2i LocationAndFilesSeparator::getPosition() {
	return _rect.position;
}

void LocationAndFilesSeparator::setRange(int minX, int maxX) {
	_minX = minX;
	_maxX = maxX;
}

void LocationAndFilesSeparator::cursorHover() {
	if (_rect.contains(cursor->_worldMousePosition) || _state == LocationAndFilesSeparatorState::Moving) {
		ElementGUI_hovered = this->shared_from_this();
	}
}

void LocationAndFilesSeparator::handleEvent(const sf::Event& event) {
	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (ElementGUI_pressed.get() == this) {
			ElementGUI_pressed = nullptr;
			_state = LocationAndFilesSeparatorState::Idle;
		}
	}
	else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_rect.contains(cursor->_worldMousePosition)) {
			ElementGUI_pressed = this->shared_from_this();
			_state = LocationAndFilesSeparatorState::Moving;
			_offset = cursor->_worldMousePosition - _rect.position;

		}
	}
}

void LocationAndFilesSeparator::update() {

	if (_state == LocationAndFilesSeparatorState::Moving) {

		sf::Vector2i newPos = cursor->_worldMousePosition - _offset;
		newPos.x = std::clamp(newPos.x, _minX, _maxX);
		_rect.position = sf::Vector2i(newPos.x, _rect.position.y);
		if (_func)
			_func();
	}
}

void LocationAndFilesSeparator::draw() {

	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setPosition(sf::Vector2f(_rect.position));
	switch (_state)
	{
	case LocationAndFilesSeparatorState::Idle:
		rect.setFillColor(file_dialog_separator_idle_color);
		break;
	case LocationAndFilesSeparatorState::Hover:
		rect.setFillColor(file_dialog_separator_hover_color);
		break;
	case LocationAndFilesSeparatorState::Moving:
		rect.setFillColor(file_dialog_separator_press_color);
		break;
	}
	window->draw(rect);
}
////////////////////////////////////////////////////////////////////////////

FileRect::FileRect() : ElementGUI(){

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));

	_state = ButtonState::Idle;
	_clickTime = currentTime;
	_onclick_func = []() {};
	
}

FileRect::~FileRect() {

}

void FileRect::setSize(sf::Vector2i size) {
	_rect.size = size;
}

void FileRect::setPosition(sf::Vector2i position) {
	_rect.position = position;
	
}

void FileRect::setFile(std::filesystem::path path) {
	_path = path;
}

void FileRect::setText(std::wstring text) {
	_textStr = text;
}

void FileRect::unclick() {
	_state = ButtonState::Idle;
}

void FileRect::hover() {
	_state = ButtonState::Hover;
}

void FileRect::click() {
	_state = ButtonState::Pressed;
	_clickTime = currentTime;
}


void FileRect::cursorHover() {

	if (_path != std::filesystem::path()) {
		if (_rect.contains(cursor->_worldMousePosition)) {
			ElementGUI_hovered = this->shared_from_this();
		}
	}
}

void FileRect::handleEvent(const sf::Event& event) {

	
	if (_path != std::filesystem::path()) {
		if (_rect.contains(cursor->_worldMousePosition)) {
			if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
				ElementGUI_pressed = this->shared_from_this();

			}
			else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
				if (ElementGUI_pressed.get() == this) {
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
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();
}

void FileRect::draw() {

	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setPosition(sf::Vector2f(_rect.position));

	switch (_state)
	{
	case ButtonState::Pressed:
		rect.setFillColor(file_dialog_file_rect_press_color);
		break;
	case ButtonState::Hover:
		rect.setFillColor(file_dialog_file_rect_hover_color);
		break;
	case ButtonState::Idle:
		rect.setFillColor(file_dialog_file_rect_idle_color);
		break;
	}
	window->draw(rect);

	sf::Sprite ico(isDrive(_path.wstring()) ? *getTexture(L"tex\\file_dialog\\harddrive.png")->_texture :
		std::filesystem::is_directory(_path) ? *getTexture(L"tex\\file_dialog\\dictionary.png")->_texture :
		_path != std::filesystem::path() ? *getTexture(L"tex\\file_dialog\\file.png")->_texture :
		*getTexture(L"tex\\file_dialog\\empty.png")->_texture);
	ico.setPosition(sf::Vector2f(_rect.position));
	window->draw(ico);

	if (!_text) {
		_text = std::make_unique<sf::Text>(basicFont, L"...", basic_text_size);
		_text->setFillColor(basic_text_color);
	}
	
	_text->setString(_textStr);
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(20 + 4, (20 - basicFont.getLineSpacing(dialog_content_font_size)) / 2.0f));
	window->draw(*_text);
	
}

//////////////////////////////////////////////////////////////////////////////

FileDialog::FileDialog(std::wstring dialogName, std::wstring selectButtonText) : Dialog(dialogName, sf::Vector2i(400, 284), sf::Vector2i(8, 120), true) {

	_files.clear();
	
	// current path = desktop
	const wchar_t* userProfile = _wgetenv(L"USERPROFILE");

	if (userProfile) {
		currentPath = userProfile;
		currentPath += L"\\Desktop";
	}
	else {
		currentPath = L"C:\\";
	}
	
	//std::wcout << L"Current path: " << currentPath << std::endl;
	createLeftPanel(9);
	createSeparator(9);
	createRightPanel(9);
	loadDirectory();
	
	setTheFiles();

	_bottomRect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(getContentSize().x, 70));
	_filenameTextWidth = 56;
	sf::Vector2i InputSize(getContentSize().x - _filenameTextWidth - 3 * dialog_padding, basic_text_rect_height);
	_filenameInput = std::make_shared<TextInput>(InputSize, 32, basic_text_size);
	
	sf::Vector2i btnSize(64, basic_text_rect_height + dialog_padding);
	_selectBtn = std::make_shared<ColoredButtonWithText>(selectButtonText, btnSize);
	_selectBtn->setRectColors(dark_button_select_color, dark_button_idle_color, dark_button_hover_color, dark_button_press_color);
	_selectBtn->_onclick_func = [this](){
		_state = DialogState::ToClose;
		};

	_cancelBtn = std::make_shared<ColoredButtonWithText>(L"Cancel", btnSize);
	_cancelBtn->setRectColors(dark_button_select_color, dark_button_idle_color, dark_button_hover_color, dark_button_press_color);
	_cancelBtn->_onclick_func = [this]() {
		_state = DialogState::ToClose;
		};

	_filenameInput->_onEnteredFunction = [this]() {
		_selectBtn->click();
		};

	setPosition(_position);
}

FileDialog::~FileDialog() {
	
}

float FileDialog::calculateLeftScrollbarHeight() {
	float hgh = 0;
	for (auto& fav : _locations)
		hgh += fav->getTotalHeight();
	hgh -= _leftRect->size.y;
	return hgh;
}

void FileDialog::createLeftPanel(int dictionariesCount) {

	sf::Vector2i rectSize = sf::Vector2i(100, dictionariesCount * basic_text_rect_height);
	_leftRect = std::make_shared<sf::IntRect>(sf::Vector2i(0,0), rectSize);

	const wchar_t* userProfile = _wgetenv(L"USERPROFILE");
	std::wstring up(userProfile);

	// dictionaries
	_locations.push_back(std::make_shared<LocationRect>(up + L"\\AppData\\Roaming\\Microsoft\\Windows\\Recent"));
	_locations.push_back(std::make_shared<LocationRect>(up + L"\\Documents"));
	_locations.push_back(std::make_shared<LocationRect>(up + L"\\Music"));
	_locations.push_back(std::make_shared<LocationRect>(up + L"\\Pictures"));
	_locations.push_back(std::make_shared<LocationRect>(up + L"\\Downloads"));
	_locations.push_back(std::make_shared<LocationRect>(up + L"\\Desktop"));
	_locations.push_back(std::make_shared<LocationRect>(up + L"\\Videos"));

	// load the harddrivers
	DWORD drives = GetLogicalDrives();
	for (int i = 0; i < 32; i++)
		if ((drives >> i) & 1) {
			//printf("%c:\\\n", 'A' + i);
			_locations.push_back(std::make_shared<LocationRect>(std::wstring(1, L'A' + i) + L":\\"));
		}

	for (int i = 0; i < _locations.size(); i++) {
		_locations[i]->setSize(sf::Vector2i(_leftRect->size.x, basic_text_rect_height));

		// KLIK W NAZWĘ -> otwórz po PRAWEJ
		_locations[i]->_onclick_location_func = [this, i]() {
			currentPath = _locations[i]->_path.wstring();
			loadDirectory();
			_rightScrollbar->setMax((int)(_filesPaths.size() - _files.size() + 1) * basic_text_rect_height);
			_rightScrollbar->setValue(0);
			setTheFiles();
			setPosition(_position);
			};

		// KLIK W STRZAŁKĘ -> tylko rozwiń/zwiń po LEWEJ
		_locations[i]->_onclick_arrow_func = [this, i]() {
			auto& node = _locations[i];
			if (!hasChildren(node->_path)) return;

			if (node->_isOpen) {
				node->close();
			}
			else {
				// UWAGA: przekazujemy onPick, żeby POTOMKOWIE mieli poprawny "open po prawej"
				node->open([this](const std::wstring& newPath) {
					currentPath = newPath;
					loadDirectory();
					_rightScrollbar->setMax((int)(_filesPaths.size() - _files.size() + 1) * basic_text_rect_height);
					_rightScrollbar->setValue(0);
					setTheFiles();
					setPosition(_position);
					});
			}

			// odśwież lewy panel (wysokość + pozycje)
			_leftScrollbar->setMax((int)calculateLeftScrollbarHeight());
			_leftScrollbar->setValue(_leftScrollbar->_value); // clamp
			setPosition(_position);
			};
	}

	// scrollbar
	sf::Vector2i scrollbarPos;
	scrollbarPos.x = _leftRect->position.x + _leftRect->size.x + dialog_padding;
	scrollbarPos.y = getContentPosition().y;

	sf::Vector2i scrollbarSize = sf::Vector2i(16, _leftRect->size.y);

	int scrollbarMax = (int)calculateLeftScrollbarHeight();
	int scrollbarSliderSize = (dictionariesCount - 1) * basic_text_rect_height;

	_leftScrollbar = std::make_shared<Scrollbar>(scrollbarPos.x, scrollbarPos.y, scrollbarSize.x, scrollbarSize.y, 0, scrollbarMax, scrollbarSliderSize, 0);
	_leftScrollbar->_func = [this]() { 
		setTheFiles();
		setPosition(_position);
		};
	_leftScrollbar->setScrollArea(_leftRect, basic_text_rect_height * 0.5f);
}

void FileDialog::createSeparator(int linesCount) {
	_separator = std::make_shared<LocationAndFilesSeparator>(linesCount);
	_separator->_rect.position = sf::Vector2i(getContentPosition().x + _leftRect->size.x + 16 + dialog_padding, getContentPosition().y);
	_separator->setRange(_separator->_rect.position.x, _separator->_rect.position.x + 128);
	_separator->_func = [this]() {
		_leftRect->size = sf::Vector2i(_separator->getPosition().x - _leftRect->position.x - 16, _leftRect->size.y);

		for(int i=0;i<_locations.size(); i++)
			_locations[i]->setSize(sf::Vector2i(_leftRect->size.x, basic_text_rect_height));

		_rightRect->size = sf::Vector2i(getContentSize().x - _leftRect->size.x - _separator->getSize().x - 2 * 16 - dialog_padding * 3, _rightRect->size.y);
		for (int i = 0; i < _files.size(); i++)
			_files[i]->setSize(sf::Vector2i(_rightRect->size.x, basic_text_rect_height));

		setPosition(_position);
		};
}

void FileDialog::createRightPanel(int linesCount) {
	
	int w = getContentSize().x - _leftRect->size.x - 2 * 16 - _separator->getSize().x - 3*dialog_padding;
	sf::Vector2i rectSize = sf::Vector2i(w, linesCount * basic_text_rect_height);
	_rightRect = std::make_shared<sf::IntRect>(sf::Vector2i(0, 0), rectSize);

	// files
	for (int i = 0; i < linesCount+1; i++) {
		std::shared_ptr<FileRect> file = std::make_shared<FileRect>();
		file->setSize(sf::Vector2i(_rightRect->size.x, basic_text_rect_height));
		_files.push_back(file);
	}

	// scrollbar
	sf::Vector2i scrollbarPos = sf::Vector2i(_position.x + getContentSize().x - 16 - dialog_padding, getContentPosition().y);
	sf::Vector2i scrollbarSize = sf::Vector2i(16, _rightRect->size.y);
	int minValue = 0;
	int maxValue = (int)(_filesPaths.size() - _files.size()) * basic_text_rect_height;
	int sliderSize = (int)(_files.size() - 1) * basic_text_rect_height;

	_rightScrollbar = std::make_shared<Scrollbar>(scrollbarPos.x, scrollbarPos.y, scrollbarSize.x, scrollbarSize.y, minValue, maxValue, sliderSize, 0);
	_rightScrollbar->_func = [this]() { 
		setTheFiles();
		setPosition(_position);
		};
	_rightScrollbar->setScrollArea(_rightRect, basic_text_rect_height * 0.5f);
}


void FileDialog::loadDirectory() {
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
	
	_rightScrollbar->setMax((int)(_filesPaths.size() - _files.size()+1)* basic_text_rect_height);
	
}

void FileDialog::setTheFiles() {

	// set the names for the file rects
	int scrollbarValue = _rightScrollbar->getValue();

	for (int i = 0; i < _files.size(); i++) {
		if (i * basic_text_rect_height + scrollbarValue < _filesPaths.size() * basic_text_rect_height) {

			std::filesystem::path path = _filesPaths[i + scrollbarValue/ basic_text_rect_height];

			_files[i]->setFile(path);
			
			if(_files[i]->_path == std::filesystem::path(currentPath).parent_path() && !isDrive(currentPath))
				_files[i]->setText(L"..");
			else 
				_files[i]->setText(path.filename().wstring());
			
			_files[i]->_onclick_func = [this, path]() {
				if (std::filesystem::is_directory(path)) {
					// is directory then open directory
					currentPath = path.wstring();
					loadDirectory();

					_rightScrollbar->setMax((int)(_filesPaths.size() - _files.size()+1)* basic_text_rect_height);
					_rightScrollbar->setValue(0);
					setTheFiles();
					setPosition(_position);

				}
				else {
					// is file then open file
					std::wcout << L"Selected file: " << path.wstring() << std::endl;
					_filenameInput->setText(path.filename().wstring());
				}
				};	
			//std::wcout << path.filename().wstring() << std::endl;
		}
		else {
			_files[i]->setText(L"");
			_files[i]->_path = std::filesystem::path();
			_files[i]->_onclick_func = []() {};
		}
	}

}

void FileDialog::setPosition(sf::Vector2i position) {

	// remember old positions for separator
	sf::Vector2i oldPos = getContentPosition();
	int oldAbsRangeMinX = _separator->_minX - getContentPosition().x;
	int oldAbsRangeMaxX = _separator->_maxX - getContentPosition().x;
	sf::Vector2i oldAbsSeparatorPos = _separator->getPosition() - oldPos;

	// set the position
	Dialog::setPosition(position);

	// rects
	_leftRect->position = sf::Vector2i(getContentPosition().x + dialog_padding, getContentPosition().y);
	_rightRect->position = sf::Vector2i(getContentPosition().x + getContentSize().x - _rightRect->size.x - 3 * dialog_padding, getContentPosition().y);
	
	_bottomRect.position.x = getContentPosition().x;
	_bottomRect.position.y = getContentPosition().y +
		std::max(_rightRect->size.y, _leftRect->size.y);


	// separator
	_separator->setPosition(getContentPosition() + oldAbsSeparatorPos);
	_separator->setRange(getContentPosition().x + oldAbsRangeMinX, getContentPosition().x + oldAbsRangeMaxX);

}

void FileDialog::drawLeftPanel() {

	sf::View view(sf::FloatRect(
		sf::Vector2f((float)_leftRect->position.x, (float)_leftRect->position.y),
		sf::Vector2f((float)_leftRect->size.x, (float)_leftRect->size.y)
	));

	sf::FloatRect vp(
		sf::Vector2f((float)_leftRect->position.x / mainView.getSize().x, (float)_leftRect->position.y / mainView.getSize().y),
		sf::Vector2f((float)_leftRect->size.x / mainView.getSize().x, (float)_leftRect->size.y / mainView.getSize().y)
	);

	view.setViewport(vp);
	window->setView(view);

	/*
	sf::RectangleShape leftRect(sf::Vector2f(_leftRect.size));
	leftRect.setPosition(sf::Vector2f(_leftRect.position));
	leftRect.setFillColor(sf::Color(255, 47, 47, 127));
	window->draw(leftRect);
	*/

	sf::Vector2i pos;
	pos.x = getContentPosition().x + dialog_padding;
	pos.y = getContentPosition().y - _leftScrollbar->getValue();
	for (int i = 0; i < _locations.size(); i++) {
		_locations[i]->setPosition(pos);
		pos.y += (int)_locations[i]->getTotalHeight();
	}

	for (auto& fav : _locations)
		fav->draw();

	window->setView(mainView);
}

void FileDialog::drawRightPanel() {

	sf::View view(sf::FloatRect(
		sf::Vector2f((float)_rightRect->position.x, (float)_rightRect->position.y),
		sf::Vector2f((float)_rightRect->size.x, (float)_rightRect->size.y)
	));

	sf::FloatRect vp(
		sf::Vector2f((float)_rightRect->position.x / mainView.getSize().x, (float)_rightRect->position.y / mainView.getSize().y),
		sf::Vector2f((float)_rightRect->size.x / mainView.getSize().x, (float)_rightRect->size.y / mainView.getSize().y)
	);

	view.setViewport(vp);
	window->setView(view);

	/*
	sf::RectangleShape rect(sf::Vector2f(_rightRect.size));
	rect.setPosition(sf::Vector2f(_rightRect.position));
	rect.setFillColor(sf::Color(255, 47, 47, 127));
	window->draw(rect);
	*/

	for (int i = 0; i < _files.size(); i++) {
		sf::Vector2i pos;
		pos.x = _separator->getPosition().x + _separator->getSize().x + dialog_padding;
		pos.x = _rightRect->position.x;
		pos.y = getContentPosition().y + (i * basic_text_rect_height) - _rightScrollbar->getValue() % basic_text_rect_height;
		//std::wcout << rightScrollbar->getValue() << " : " << pos.y << L"\n";
		_files[i]->setPosition(pos);
	}

	for (auto& file : _files)
		file->draw();

	window->setView(mainView);
}

void FileDialog::drawBottomPanel() {

	sf::View view(sf::FloatRect(
		sf::Vector2f((float)_bottomRect.position.x, (float)_bottomRect.position.y),
		sf::Vector2f((float)_bottomRect.size.x, (float)_bottomRect.size.y)
	));

	sf::FloatRect vp(
		sf::Vector2f((float)_bottomRect.position.x / mainView.getSize().x, (float)_bottomRect.position.y / mainView.getSize().y),
		sf::Vector2f((float)_bottomRect.size.x / mainView.getSize().x, (float)_bottomRect.size.y / mainView.getSize().y)
	);

	view.setViewport(vp);
	window->setView(view);

	// selected file name box
	sf::RectangleShape rect(sf::Vector2f(_bottomRect.size));
	rect.setFillColor(dialog_content_rect_color_2);
	rect.setPosition(sf::Vector2f(_bottomRect.position));
	window->draw(rect);

	// filename text
	if (_filenameText == nullptr) {
		_filenameText = std::make_unique<sf::Text>(basicFont, L"File name", basic_text_size);
		_filenameText->setFillColor(basic_text_color);
	}
	sf::Vector2f filenameTextPos;
	filenameTextPos.x = (float)(_bottomRect.position.x + dialog_padding);
	filenameTextPos.y = (float)(_bottomRect.position.y + dialog_padding);
	_filenameText->setPosition(filenameTextPos);
	window->draw(*_filenameText);

	// filename input
	sf::Vector2i filenameInputPos;
	filenameInputPos.x = getContentPosition().x + _filenameTextWidth + 2 * dialog_padding;
	filenameInputPos.y = _bottomRect.position.y + dialog_padding;
	_filenameInput->setPosition(filenameInputPos);
	_filenameInput->draw();

	// buttons
	sf::Vector2i btnPos;
	btnPos.x = getContentPosition().x + getContentSize().x - _cancelBtn->getSize().x - dialog_padding;
	btnPos.y = _filenameInput->getPosition().y + _filenameInput->getSize().y + dialog_padding;
	_cancelBtn->setPosition(btnPos);

	btnPos.x -= dialog_padding + _selectBtn->getSize().x;
	_selectBtn->setPosition(btnPos);
	_selectBtn->draw();
	_cancelBtn->draw();

	window->setView(mainView);
}

void FileDialog::cursorHoverLocations(std::shared_ptr<LocationRect> location)
{
	location->cursorHover();

	if (location->_isOpen) {
		for (auto& child : location->_children) {
			cursorHoverLocations(child);
		}
	}
}

void FileDialog::handleEventLocations(std::shared_ptr<LocationRect> location, const sf::Event& event)
{
	location->handleEvent(event);

	if (location->_isOpen) {
		for (auto& child : location->_children) {
			handleEventLocations(child, event);
		}
	}

}

void FileDialog::updateLocations(std::shared_ptr<LocationRect> location)
{
	location->update();

	if (location->_isOpen) {
		for (auto& child : location->_children) {
			updateLocations(child);
		}
	}

	_filenameInput->update();
}

void FileDialog::cursorHover() {
	Dialog::cursorHover();

	if (_rightScrollbar->_state == ScrollbarState::Idle && _leftScrollbar->_state == ScrollbarState::Idle) {
		if (_rightRect->contains(cursor->_worldMousePosition)) {
			for (auto& file : _files)
				file->cursorHover();
		}
		
		for (auto& fav : _locations) {
			cursorHoverLocations(fav);
		}
	}
		
	_leftScrollbar->cursorHover();
	_rightScrollbar->cursorHover();

	_separator->cursorHover();

	_filenameInput->cursorHover();

	_selectBtn->cursorHover();
	_cancelBtn->cursorHover();
}

void FileDialog::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_leftScrollbar->handleEvent(event);
	_rightScrollbar->handleEvent(event);

	if (_rightRect->contains(cursor->_worldMousePosition)) {
		for (auto& file : _files)
			file->handleEvent(event);
	}

	if (_leftRect->contains(cursor->_worldMousePosition)) {
		for (auto& fav : _locations) {
			handleEventLocations(fav, event);
		}
	}

	_separator->handleEvent(event);

	_filenameInput->handleEvent(event);

	_selectBtn->handleEvent(event);
	_cancelBtn->handleEvent(event);
}

void FileDialog::update() {
	Dialog::update();

	for (auto& file : _files)
		file->update();

	for (auto& fav : _locations) {
		updateLocations(fav);
	}

	// DODAJ TO (odśwież max i clamp wartości po ewentualnym expand/collapse):
	_leftScrollbar->setMax((int)calculateLeftScrollbarHeight());
	_leftScrollbar->setValue(_leftScrollbar->_value); // clamp

	_leftScrollbar->update();
	_rightScrollbar->update();

	_separator->update();

	_filenameInput->update();

	_selectBtn->update();
	_cancelBtn->update();
}

void FileDialog::draw() {
	Dialog::draw();

	drawLeftPanel();
	drawRightPanel();

	// left scrollbar
	sf::Vector2i leftScrollbarPos(_leftRect->position.x + _leftRect->size.x, _leftRect->position.y);
	_leftScrollbar->setPosition(leftScrollbarPos);
	_leftScrollbar->draw();

	// right scrollbar
	sf::Vector2i rightScrollbarPos(getContentPosition().x + getContentSize().x - 16 - dialog_padding, getContentPosition().y);
	_rightScrollbar->setPosition(rightScrollbarPos);
	_rightScrollbar->draw();

	_separator->draw();

	drawBottomPanel();

}