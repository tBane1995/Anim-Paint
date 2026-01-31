#include "Dialogs/Palette.hpp"
#include "Filters.hpp"
#include "Time.hpp"
#include "Window.hpp"
#include "Cursor.hpp"
#include "Tools/Toolbar.hpp"
#include "SFML/Graphics.hpp"
#include "Theme.hpp"
#include <iostream>
#include "MainMenu.hpp"
#include "BottomBar.hpp"
#include "DebugLog.hpp"

HSV rgbToHsv(sf::Color c) {
	float r = c.r / 255.f, g = c.g / 255.f, b = c.b / 255.f;
	float mx = std::max({ r,g,b }), mn = std::min({ r,g,b });
	float d = mx - mn;

	float h = 0.f;
	if (d > 0.f) {
		if (mx == r)       h = fmodf((g - b) / d, 6.f);
		else if (mx == g)  h = (b - r) / d + 2.f;
		else               h = (r - g) / d + 4.f;
		h /= 6.f;
		if (h < 0.f) h += 1.f;
	}
	float s = (mx == 0.f) ? 0.f : (d / mx);
	float v = mx;
	return { h, s, v };
}

sf::Vector2i cursorOnHues(sf::Vector2i rectSize, sf::Color rgb) {
	HSV hsv = rgbToHsv(rgb);

	float x = std::clamp(hsv.h, 0.f, 1.f);     // H -> x
	float y = std::clamp(1.0f - hsv.s, 0.f, 1.f); // S -> 1 - y

	int ix = std::clamp(int(std::floor(x * (rectSize.x - 1))), 0, rectSize.x - 1);
	int iy = std::clamp(int(std::floor(y * (rectSize.y - 1))), 0, rectSize.y - 1);

	return sf::Vector2i(ix, iy);
}

sf::Vector2i cursorOnValues(sf::Vector2i rectSize, sf::Color rgb) {
	HSV hsv = rgbToHsv(rgb);

	float y = std::clamp(1.f - hsv.v, 0.f, 1.f);

	int ix = 0;
	float fy = std::clamp(float(y)*(rectSize.y-1), 0.0f, float(rectSize.y - 1));
	int iy = (int)(fy);

	return sf::Vector2i(ix, iy);
}

PaletteValues::PaletteValues(sf::Vector2i position, sf::Vector2i size, std::string shader, sf::Color color) {
	
	_shader = sf::Shader();

	if (!_shader.loadFromMemory(shader, sf::Shader::Type::Fragment)) {
		DebugError(L"PaletteValues::PaletteValues: Failed to load palette shader from memory.");
		exit(0);
	}

	_rect = sf::IntRect(position, size);

	loadTexture(color);

	_state = PaletteState::None;
}

PaletteValues::~PaletteValues() {

}

void PaletteValues::loadTexture(sf::Color color) {
	// create image && texture
	sf::Image img;
	img.resize(sf::Vector2u(_rect.size), color);

	sf::Texture tex;
	if (!tex.loadFromImage(img)) {
		DebugError(L"PaletteValues::loadTexture: Failed to load texture from image.");
		exit(0);
	}

	// set the uniforms in shader
	sf::IntRect r = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(tex.getSize().x, tex.getSize().y));
	sf::Vector2u ts = tex.getSize(); 

	sf::Glsl::Vec4 texRectUV(
		float(r.position.x) / ts.x,
		float(r.position.y) / ts.y,
		float(r.position.x + r.size.x) / ts.x,
		float(r.position.y + r.size.y) / ts.y
	);

	_shader.setUniform("texRectUV", texRectUV);

	// create palette render texture
	if (!_renderTexture.resize(tex.getSize())) {
		DebugError(L"PaletteValues::loadTexture: Failed to resize render texture.");
		exit(0);
	}

	sf::Sprite spr(tex);
	_renderTexture.clear(sf::Color::White);
	_renderTexture.draw(spr, &_shader);
	_renderTexture.display();
}

void PaletteValues::setPosition(sf::Vector2i position) {
	_rect.position = position;
}

void PaletteValues::cursorHover() {
	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}
}

void PaletteValues::handleEvent(const sf::Event& event) {

	if (!dialogs.empty())
		return;

	if (main_menu->_open_menu_box != nullptr)
		return;

	if(const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		_state = PaletteState::None;
		if(ElementGUI_pressed.get() == this)
			ElementGUI_pressed = nullptr;
	}
	
	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			_state = PaletteState::Selecting;
			ElementGUI_pressed = this->shared_from_this();
		}

		if (const auto* mbp = event.getIf<sf::Event::MouseMoved>(); ElementGUI_pressed.get() == this && mbp && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
			_state = PaletteState::Selecting;
			ElementGUI_pressed = this->shared_from_this();
		}

		
	}
}

void PaletteValues::update() {
	if (_state == PaletteState::Selecting && ElementGUI_pressed.get() == this) {
		_function();
	}


}

void PaletteValues::draw() {
	sf::Sprite sprite(_renderTexture.getTexture());
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);
}

Palette::Palette() : Dialog(L"Palette", sf::Vector2i(192 + 24 + 8, dialog_title_rect_height + 192 + basic_text_rect_height + 8)){

	sf::Vector2i size = sf::Vector2i(sf::Vector2u(sf::Vector2i(192-16, 192-16)));
	sf::Vector2i position = getContentPosition() + sf::Vector2i(8, 8);
	_hues = std::make_shared<PaletteValues>(position, size, palette_colors_shader_source, sf::Color::White);
	
	position = sf::Vector2i(_hues->_rect.position + sf::Vector2i(_hues->_rect.size.x + 8, 0));
	size.x = 24;
	_values = std::make_shared<PaletteValues>(position, size, palette_values_shader_source, sf::Color::White);

	_hues->_function = [this]() {
		sf::Image pixels = _hues->_renderTexture.getTexture().copyToImage();

		_huesCursor.x = std::clamp(cursor->_worldMousePosition.x - _hues->_rect.position.x, 0, _hues->_rect.size.x - 1);
		_huesCursor.y = std::clamp(cursor->_worldMousePosition.y - _hues->_rect.position.y, 0, _hues->_rect.size.y - 1);

		sf::Color color = pixels.getPixel(sf::Vector2u(_huesCursor));
		_values->loadTexture(color);

		sf::Color finalColor = _values->_renderTexture.getTexture().copyToImage().getPixel(sf::Vector2u(_valuesCursor));
		toolbar->_selectedColorButton->setColor(finalColor);
		_red->setText(std::to_wstring(finalColor.r));
		_green->setText(std::to_wstring(finalColor.g));
		_blue->setText(std::to_wstring(finalColor.b));
		};

	_values->_function = [this]() {
		sf::Image pixels = _values->_renderTexture.getTexture().copyToImage();

		_valuesCursor.x = 0;
		_valuesCursor.y = std::clamp(cursor->_worldMousePosition.y - _values->_rect.position.y, 0, _values->_rect.size.y - 1);

		sf::Color color = pixels.getPixel(sf::Vector2u(_valuesCursor));
		toolbar->_selectedColorButton->setColor(color);

		_red->setText(std::to_wstring(color.r));
		_green->setText(std::to_wstring(color.g));
		_blue->setText(std::to_wstring(color.b));
		};

	_r = std::make_unique<sf::Text>(basicFont, L"R", basic_text_size);
	_g = std::make_unique<sf::Text>(basicFont, L"G", basic_text_size);
	_b = std::make_unique<sf::Text>(basicFont, L"B", basic_text_size);

	_r->setFillColor(basic_text_color);
	_g->setFillColor(basic_text_color);
	_b->setFillColor(basic_text_color);

	_red = std::make_shared<NumberInput>(sf::Vector2i(32, basic_text_rect_height), 3, basic_text_size, 0, 0, 255);
	_green = std::make_shared<NumberInput>(sf::Vector2i(32, basic_text_rect_height), 3, basic_text_size, 0, 0, 255);
	_blue = std::make_shared<NumberInput>(sf::Vector2i(32, basic_text_rect_height), 3, basic_text_size, 0, 0, 255);
	
	_red->setText(std::to_wstring(toolbar->_first_color->_color.r));
	_green->setText(std::to_wstring(toolbar->_first_color->_color.g));
	_blue->setText(std::to_wstring(toolbar->_first_color->_color.b));
	
	_onTabElements.push_back(_red);
	_onTabElements.push_back(_green);
	_onTabElements.push_back(_blue);

	_red->_onClickedFunction = [this]() {activateOnTabElement(0); loadColorFromRGBInputs(); };
	_green->_onClickedFunction = [this]() {activateOnTabElement(1); loadColorFromRGBInputs(); };
	_blue->_onClickedFunction = [this]() {activateOnTabElement(2); loadColorFromRGBInputs(); };

	_red->_onEditedFunction = [this]() {loadColorFromRGBInputs();};
	_green->_onEditedFunction = [this]() {loadColorFromRGBInputs(); };
	_blue->_onEditedFunction = [this]() {loadColorFromRGBInputs(); };

	_red->_onEnteredFunction = [this]() {activateOnTabElement(1); loadColorFromRGBInputs(); };
	_green->_onEnteredFunction = [this]() {activateOnTabElement(2); loadColorFromRGBInputs(); };
	_blue->_onEnteredFunction = [this]() {activateOnTabElement(0); loadColorFromRGBInputs(); };

	setPosition(_position);

	loadColorFromRGBInputs();
	
	
}

Palette::~Palette() {
	
}

void Palette::setColorInRGBInputs(sf::Color color) {
	_red->setText(std::to_wstring(color.r));
	_green->setText(std::to_wstring(color.g));
	_blue->setText(std::to_wstring(color.b));
}

void Palette::loadColorFromRGBInputs() {

	sf::Color color = sf::Color(
		std::stoi(_red->getText()),
		std::stoi(_green->getText()),
		std::stoi(_blue->getText()),
		255
	);

	_huesCursor = cursorOnHues(_hues->_rect.size, color);
	_values->loadTexture(_hues->_renderTexture.getTexture().copyToImage().getPixel(sf::Vector2u(_huesCursor)));
	_valuesCursor = cursorOnValues(_values->_rect.size, color);

}

void Palette::setPosition(sf::Vector2i position) {
	Dialog::setPosition(position);
	_hues->setPosition(getContentPosition() + sf::Vector2i(8, 8));
	_values->setPosition(sf::Vector2i(_hues->_rect.position + sf::Vector2i(_hues->_rect.size.x + 8, 0)));

	sf::Vector2i textInputsPosistion;
	textInputsPosistion.x = position.x + 32;
	textInputsPosistion.y = _values->_rect.position.y + _values->_rect.size.y + 8;

	_red->setPosition(textInputsPosistion);
	_green->setPosition(textInputsPosistion + sf::Vector2i(32 * 2, 0));
	_blue->setPosition(textInputsPosistion + sf::Vector2i(32 * 4, 0));

	_r->setPosition(sf::Vector2f(_red->_text->getPosition()) - sf::Vector2f(16, 0));
	_g->setPosition(sf::Vector2f(_green->_text->getPosition()) - sf::Vector2f(16, 0));
	_b->setPosition(sf::Vector2f(_blue->_text->getPosition()) - sf::Vector2f(16, 0));
}

void Palette::cursorHover() {
	if (!dialogs.empty())
		return;

	Dialog::cursorHover();
	_hues->cursorHover();
	_values->cursorHover();
	
	_red->cursorHover();
	_green->cursorHover();
	_blue->cursorHover();
}

void Palette::handleEvent(const sf::Event& event) {
	if (!dialogs.empty())
		return;

	Dialog::handleEvent(event);

	if (!Dialog::_is_moved) {
		clampPosition();
	}

	_hues->handleEvent(event);
	_values->handleEvent(event);

	// Special handling for Enter events.
	// Only the currently focused NumberInput should receive the Enter key.
	// Without this, the event would propagate to all NumberInputs,
	// causing multiple fields to respond to a single Enter press.
	// We route Enter exclusively to the active element to avoid that.

	// Handle Enter key for NumberInput elements
	if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp && kp->code == sf::Keyboard::Key::Enter) {
		if (_currentOnTabElement >= 0 && _currentOnTabElement < (int)_onTabElements.size()) {
			if (auto ni = std::dynamic_pointer_cast<NumberInput>(_onTabElements[_currentOnTabElement])) {
				ni->handleEvent(event);
				return;
			}
		}
		
	}

	// Handle TextEntered event for Enter key (in case of IME input)
	if (const auto* te = event.getIf<sf::Event::TextEntered>(); te && (te->unicode == 13 || te->unicode == 10)) {
		if (_currentOnTabElement >= 0 && _currentOnTabElement < (int)_onTabElements.size()) {
			if (auto ni = std::dynamic_pointer_cast<NumberInput>(_onTabElements[_currentOnTabElement])) {
				ni->handleEvent(event);
				return;
			}
		}
		
	}

	_red->handleEvent(event);
	_green->handleEvent(event);
	_blue->handleEvent(event);
}

void Palette::update() {
	Dialog::update();

	_hues->update();
	_values->update();

	_red->update();
	_green->update();
	_blue->update();
}

void Palette::draw() {
	Dialog::draw();
	_hues->draw();
	_values->draw();

	_red->draw();
	_green->draw();
	_blue->draw();

	window->draw(*_r);
	window->draw(*_g);
	window->draw(*_b);

	sf::CircleShape hCursor(4);
	hCursor.setFillColor(sf::Color::Transparent);
	hCursor.setOutlineThickness(3);
	hCursor.setOutlineColor(sf::Color::Red);
	hCursor.setOrigin(sf::Vector2f(4, 4));

	sf::RectangleShape vCursor(sf::Vector2f(28, 4));
	vCursor.setFillColor(sf::Color::Red);
	vCursor.setOrigin(sf::Vector2f(2, 2));

	hCursor.setPosition(sf::Vector2f(_hues->_rect.position + _huesCursor));
	vCursor.setPosition(sf::Vector2f(_values->_rect.position + _valuesCursor));

	window->draw(hCursor);
	window->draw(vCursor);
	
}

std::shared_ptr<Palette> palette;