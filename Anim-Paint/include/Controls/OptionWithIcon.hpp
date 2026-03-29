#pragma once
#include "Controls/Option.hpp"

class OptionWithIcon : public Option {
public:
	std::shared_ptr<Texture> _icon;
	std::shared_ptr<Texture> _icon_hover;

	OptionWithIcon(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, std::wstring shortcut = L"", sf::Vector2i position = sf::Vector2i(0, 0));
	~OptionWithIcon();

	void draw();
};