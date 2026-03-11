#include "Dialogs/FileDialog/FileRect.hpp"
#include "Controls/Button.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Theme.hpp"



FileRect::FileRect() : Element(){

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));

	_state = ButtonState::Idle;
	_clickTime = currentTime;
	_onclick_func = []() {};

	_type = FileType::Empty;
	
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
	
	if(isDrive(_path.wstring()))
		_type = FileType::Drive;
	else if(std::filesystem::is_directory(_path))
		_type = FileType::Directory;
	else if(_path != std::filesystem::path())
		_type = FileType::File;
	else
		_type = FileType::Empty;
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
		if (_rect.contains(cursor->_position)) {
			Element_hovered = this->shared_from_this();
		}
	}
}

void FileRect::handleEvent(const sf::Event& event) {

	
	if (_path != std::filesystem::path()) {
		if (_rect.contains(cursor->_position)) {
			if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
				Element_pressed = this->shared_from_this();

			}
			else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
				if (Element_pressed.get() == this) {
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

			if (Element_pressed.get() == this)
				Element_pressed = nullptr;
			unclick();
		}
	}
	else if (Element_hovered.get() == this) {
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

	sf::Sprite ico(
		_type == FileType::Drive ? *getTexture(L"tex\\file_dialog\\harddrive.png")->_texture :
		_type == FileType::Directory ? *getTexture(L"tex\\file_dialog\\dictionary.png")->_texture :
		_type == FileType::File ? *getTexture(L"tex\\file_dialog\\file.png")->_texture :
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
