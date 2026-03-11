#pragma once
#include "Element.hpp"
#include <functional>

enum class LocationAndFilesSeparatorState { Idle, Hover, Moving };

class LocationAndFilesSeparator : public Element {
public:
	sf::IntRect _rect;
	int _minX;
	int _maxX;

	LocationAndFilesSeparatorState _state;
	sf::Vector2i _offset;
	std::function<void()> _func;

	LocationAndFilesSeparator(int linesCount);
	~LocationAndFilesSeparator();

	sf::Vector2i getSize();
	void setPosition(sf::Vector2i position);
	sf::Vector2i getPosition();
	void setRange(int minX, int maxX);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};
