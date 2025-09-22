#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "ElementGUI/Button.hpp"

enum class DialogState { Idle, ToClose };

class Dialog : public ElementGUI {
public:
	sf::Vector2f _position;
	sf::RectangleShape _dialog_rect;
	sf::RectangleShape _title_rect;
	sf::Text _title_text;
	sf::RectangleShape _content_rect;
	NormalButton* _close_btn;

	DialogState _state;

	bool _is_moved;
	sf::Vector2f _offset;

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
