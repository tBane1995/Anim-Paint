#pragma once
#include "Controls/Option.hpp"
#include "Controls/Checkbox.hpp"

class OptionWithCheckbox : public Option {
public:
	std::shared_ptr<Checkbox> _checkbox;

	OptionWithCheckbox(std::wstring text, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0));
	~OptionWithCheckbox();

	void addValue(std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture);
	void setPosition(sf::Vector2i position, int shortcut_offset);
	void click();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};