#pragma once
#include "ElementGUI.hpp"
#include "Button.hpp"

enum class DialogState { Idle, ToClose };

class Dialog : public ElementGUI {
public:
	sf::Vector2f position;
	sf::RectangleShape dialog_rect;
	sf::RectangleShape title_rect;
	sf::Text title_text;
	sf::RectangleShape content_rect;
	NormalButton* close_btn;

	DialogState state;

	bool is_moved;
	sf::Vector2f offset;

	Dialog(std::wstring title, sf::Vector2f size, sf::Vector2f position = sf::Vector2f(-1, -1));
	virtual ~Dialog();

	sf::Vector2f getPosition();
	void setSize(sf::Vector2f size);
	sf::Vector2f getSize();
	virtual void setPosition(sf::Vector2f position);

	virtual void cursorHover();
	virtual void handleEvent(sf::Event& event);
	virtual void update();
	virtual void draw();
};

extern std::vector < Dialog* > dialogs;
