#include "Dialogs/FileDialog/FileDialog.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Cursor.hpp"
#include "Time.hpp"
#include "Windows.h"
#include "Dialogs/FileDialog/FileFunctions.hpp"


//////////////////////////////////////////////////////////////////////////////

FileDialog::FileDialog(std::wstring dialogName, std::wstring selectButtonText, std::wstring acceptableExtention) : Dialog(dialogName, sf::Vector2i(400, 284), sf::Vector2i(8, 120), true) {

	_acceptableExtension = acceptableExtention;

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
			if (!node->_hasChildren) return;

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

		if(!(std::filesystem::is_directory(p) || _acceptableExtension.empty() || isExtension(name, _acceptableExtension)))
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
			_files[i]->_type = FileType::Empty;
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

	//_filenameInput->update();
}

void FileDialog::cursorHover() {
	Dialog::cursorHover();

	if (_rightScrollbar->_state == ScrollbarState::Idle && _leftScrollbar->_state == ScrollbarState::Idle) {
		if (_rightRect->contains(cursor->_position)) {
			for (auto& file : _files)
				file->cursorHover();
		}
		
		if (_leftRect->contains(cursor->_position)) {
			for (auto& fav : _locations) {
				cursorHoverLocations(fav);
			}
		}
	}
		
	_leftScrollbar->cursorHover();
	_rightScrollbar->cursorHover();

	_separator->cursorHover();

	_selectBtn->cursorHover();
	_cancelBtn->cursorHover();

	_filenameInput->cursorHover(); // at the end, because input can overwrite hovered
}

void FileDialog::handleEvent(const sf::Event& event) {
	Dialog::handleEvent(event);

	_leftScrollbar->handleEvent(event);
	_rightScrollbar->handleEvent(event);

	if (_rightRect->contains(cursor->_position)) {
		for (auto& file : _files)
			file->handleEvent(event);
	}

	if (_leftRect->contains(cursor->_position)) {
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

	window->setView(mainView);
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