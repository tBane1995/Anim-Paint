#include "Dialogs/FileDialog/LocationRect.hpp"
#include "Dialogs/FileDialog/FileFunctions.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Theme.hpp"

LocationRect::LocationRect(std::wstring path, int depth) : Element() {

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));
	unclick();

	_path = getPath(path);
	_depth = depth;

	_isOpen = false;

	if (isDrive(_path.wstring())) {
		_type = FileType::Drive;
	}
	else if (std::filesystem::is_directory(_path))
		_type = FileType::Directory;
	else if (_path != std::filesystem::path())
		_type = FileType::File;
	else
		_type = FileType::Empty;

	if(_type == FileType::Directory || _type == FileType::Drive)
		_hasChildren = hasChildren(_path);
	else
		_hasChildren = false;

	_clickType = LocationRectClickType::None;

	if (_hasChildren)
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
	if (!_hasChildren)
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
				if (!child->_hasChildren) return;
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
	
	
	if (_rect.contains(cursor->_position)) {
		Element_hovered = this->shared_from_this();
	}

	if (_isOpen && !_children.empty()) {
		for (auto& child : _children) {
			child->cursorHover();
		}
	}

}

void LocationRect::handleEvent(const sf::Event& event) {
	
	
	if (_rect.contains(cursor->_position)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {

			float indent = _rect.position.x + _depth * _indentDelta;

			if ( indent < cursor->_position.x && cursor->_position.x < indent + _arrowMargin + 8) {
				Element_pressed = this->shared_from_this();
				_clickType = LocationRectClickType::ClickArrow;
			}
			else {
				Element_pressed = this->shared_from_this();
				_clickType = LocationRectClickType::ClickLocation;
			}
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			if (Element_pressed == this->shared_from_this()) {
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


	sf::Sprite arrow(*getTexture(_isOpen ? L"tex\\file_dialog\\dictionaryIsOpened.png" : _hasChildren ? L"tex\\file_dialog\\dictionaryIsClosed.png" : L"tex\\file_dialog\\dictionaryIsEmpty.png")->_texture);
	arrow.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(indent, 6));
	window->draw(arrow);

	sf::Sprite ico(*getTexture(_type == FileType::Drive ? L"tex\\file_dialog\\harddrive.png" : _type == FileType::Directory ? L"tex\\file_dialog\\dictionary.png" : L"tex\\file_dialog\\empty.png")->_texture);
	ico.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(indent + 8 + _arrowMargin, 0));
	window->draw(ico);

	if (_text == nullptr) {
		_text = std::make_unique<sf::Text>(basicFont, L"", basic_text_size);
		_text->setFillColor(basic_text_color);
		_text->setString((_type == FileType::Drive ? _path.wstring() : _path.filename().wstring()));
	}
	_text->setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(indent + 20 + 4 + 8 + _arrowMargin, (20 - basicFont.getLineSpacing(dialog_content_font_size)) / 2.0f));
	window->draw(*_text);

	if (_isOpen) {
		for (auto& children : _children) {
			children->draw();
		}
	}

}