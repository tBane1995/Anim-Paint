#include "Components/Toolbar/PaletteButton.hpp"
#include "DebugLog.hpp"
#include "Tools/Filters.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Window.hpp"

PaletteButton::PaletteButton(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position) : ButtonWithBottomText(text, rectColor, textColor, hoverTextColor, texture, hoverTexture, position) {
	_shader = sf::Shader();
	if (!_shader.loadFromMemory(palette_button_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"PaletteButton::PaletteButton: failed to load shader from memory.");
		exit(0);
	}
}

PaletteButton::~PaletteButton() {

}

void PaletteButton::draw() {

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x - 2 * tools_border_width);
	rectSize.y = float(_rect.size.y - 2 * tools_border_width);

	sf::RectangleShape rect(rectSize);

	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(_rectPressColor);
		rect.setOutlineThickness((float)(_rectBorderWidth));
		rect.setOutlineColor(_rectPressBorderColor);
		break;
	case ButtonState::Hover:
		rect.setFillColor(_rectHoverColor);
		rect.setOutlineThickness((float)(_rectBorderWidth));
		rect.setOutlineColor(_rectHoverBorderColor);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(_rectSelectColor);
			rect.setOutlineThickness((float)(_rectBorderWidth));
			rect.setOutlineColor(_rectSelectBorderColor);
		}
		else {
			rect.setFillColor(_rectIdleColor);
			rect.setOutlineThickness((float)(_rectBorderWidth));
			rect.setOutlineColor(_rectIdleBorderColor);
		};
		break;
	};

	sf::Vector2f rectPosition;
	rectPosition.x = float(_rect.position.x + _rectBorderWidth);
	rectPosition.y = float(_rect.position.y + _rectBorderWidth);
	rect.setPosition(rectPosition);

	window->draw(rect);
	
	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	sf::RenderStates rs;
	
	_shader.setUniform("iTime", float(currentTime.asSeconds()));

	const sf::Texture& tex = (_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture;
	sf::IntRect r = sprite.getTextureRect();          // px w atlasie
	sf::Vector2u ts = tex.getSize();                  // rozmiar atlasu

	sf::Glsl::Vec4 texRectUV(
		float(r.position.x) / ts.x,
		float(r.position.y) / ts.y,
		float(r.position.x + r.size.x) / ts.x,
		float(r.position.y + r.size.y) / ts.y
	);
	
	_shader.setUniform("texRectUV", texRectUV);
	rs.shader = &_shader;
	window->draw(sprite, rs);

	window->draw(*_text);
}