#pragma once
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "Textures.hpp"
#include <functional>

enum class ButtonState { Idle, Hover, Pressed };

class Button : public Element {
public:
	//sf::RectangleShape _rect;
	sf::IntRect _rect;
	bool _isSelected;
	bool _activatedByEnter;

	sf::Color _rectIdleColor;
	sf::Color _rectHoverColor;
	sf::Color _rectPressColor;
	sf::Color _rectSelectColor;

	int _rectBorderWidth;
	sf::Color _rectIdleBorderColor;
	sf::Color _rectHoverBorderColor;
	sf::Color _rectPressBorderColor;
	sf::Color _rectSelectBorderColor;

	ButtonState _state;
	std::function<void()> _hover_func;
	std::function<void()> _press_func;
	std::function<void()> _onclick_func;
	sf::Time _clickTime;

	std::wstring _title;
	std::wstring _description;

	Button();
	virtual ~Button();

	void setRectColors(sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor, sf::Color selectColor);
	void setRectColors(sf::Color idleColor, sf::Color hoverColor, sf::Color pressColor, sf::Color selectColor, int borderWidth, sf::Color idleBorderColor, sf::Color hoverBorderColor, sf::Color pressBorderColor, sf::Color selectBorderColor);
	
	virtual void setSize(sf::Vector2i size);
	virtual sf::Vector2i getSize();
	virtual sf::Vector2i getPosition();
	virtual void setPosition(sf::Vector2i position);

	virtual void select();
	virtual void unselect();
	virtual void unclick();
	virtual void hover();
	virtual void click();

	virtual void activateByEnter(bool activate);

	virtual void setTooltip(std::wstring title, std::wstring description);

	

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();

};



