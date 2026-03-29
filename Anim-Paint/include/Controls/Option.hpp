#pragma once
#include "Controls/Button.hpp"

class Option : public Button {
public:
	std::unique_ptr<sf::Text> _text;
	std::unique_ptr<sf::Text> _shortcut_text;


	Option(std::wstring text, std::wstring shortcut = L"", sf::Vector2i position = sf::Vector2i(0, 0));
	~Option();

	virtual void setPosition(sf::Vector2i position, int shortcut_offset);
	virtual int getTextWidth();
	virtual int getShortcutTextWidth();
	virtual void draw();

};