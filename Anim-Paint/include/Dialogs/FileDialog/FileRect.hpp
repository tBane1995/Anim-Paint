#pragma once
#include "Element.hpp"
#include <filesystem>
#include "Controls/Button.hpp"
#include "Dialogs/FileDialog/FileFunctions.hpp"

class FileRect : public Element {
public:
	std::filesystem::path _path;
	FileType _type;
	sf::IntRect _rect;
	std::unique_ptr<sf::Text> _text;
	std::wstring _textStr;
	ButtonState _state;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	FileRect();
	~FileRect();

	void setSize(sf::Vector2i size);
	void setPosition(sf::Vector2i position);
	void setFile(std::filesystem::path path);
	void setText(std::wstring text);
	
	void unclick();
	void hover();
	void click();
	
	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};