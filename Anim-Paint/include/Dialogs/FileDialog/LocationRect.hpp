#pragma once
#include "Element.hpp"
#include "Dialogs/FileDialog/FileFunctions.hpp"
#include <functional>
#include "Controls/Button.hpp"


enum class LocationRectClickType { None, ClickArrow, ClickLocation };

class LocationRect : public Element {
public:

	const float _arrowMargin = 4;
	const float _indentDelta = 12;

	std::filesystem::path _path;
	int _depth;
	sf::IntRect _rect;
	std::unique_ptr<sf::Text> _text;

	bool _isOpen;
	FileType _type;
	bool _hasChildren;
	std::vector <std::shared_ptr<LocationRect>> _children;

	ButtonState _state;
	LocationRectClickType _clickType;
	std::function<void()> _onclick_arrow_func;
	std::function<void()> _onclick_location_func;
	sf::Time _clickTime;

	LocationRect(std::wstring path, int depth = 0);
	~LocationRect();

	void setSize(sf::Vector2i size);
	float getTotalHeight();
	void setPosition(sf::Vector2i position);
	void setText(std::wstring text);
	
	void open(const std::function<void(const std::wstring&)>& onPick);	
	void close();
	void hide();

	void unclick();
	void hover();
	void locationClick();
	void arrowClick();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();

};
