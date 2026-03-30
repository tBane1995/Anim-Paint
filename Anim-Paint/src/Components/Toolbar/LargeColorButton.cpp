#include "Components/Toolbar/LargeColorButton.hpp"
#include "Window.hpp"
#include "Tools/Filters.hpp"
#include "DebugLog.hpp"

LargeColorButton::LargeColorButton(sf::Color color) : ButtonWithSprite(getTexture(L"tex\\tools\\frame_large.png"), getTexture(L"tex\\tools\\frame_large_hover.png")) {
	_color = color;

    _shader = sf::Shader();

    if (!_shader.loadFromMemory(color_on_chessboard_shader_source, sf::Shader::Type::Fragment)) {
        DebugError(L"LargeColorButton::LargeColorButton: Failed to load color on chessboard shader from memory.");
        exit(0);
    }
}

LargeColorButton::~LargeColorButton() {

}

void LargeColorButton::setColor(sf::Color color) {
	_color = color;
}

void LargeColorButton::draw() {
	ButtonWithSprite::draw();

    sf::RectangleShape color_rect(sf::Vector2f(40, 40));
    color_rect.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(4, 4));

    _shader.setUniform("color", sf::Glsl::Vec4((float)_color.r/255.f, (float)_color.g/255.f, (float)_color.b/255.f, (float)_color.a/255.f));
    _shader.setUniform("rectPos", sf::Glsl::Vec2(color_rect.getPosition()));
    _shader.setUniform("rectSize", sf::Glsl::Vec2(color_rect.getSize()));

    window->draw(color_rect, &_shader);

}