#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "ElementGUI/Button.hpp"

enum class DialogState { Idle, ToClose };

class Dialog : public ElementGUI {
public:
	sf::Vector2i _position;
	std::wstring _title;

	sf::IntRect _dialogRect;
	sf::IntRect _titleRect;
	std::unique_ptr<sf::Text> _titleText;
	sf::IntRect _contentRect;
	NormalButton* _closeBtn;

	DialogState _state;

	bool _is_moved;
	sf::Vector2i _offset;

	Dialog(std::wstring title, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(-1, -1));
	virtual ~Dialog();

	sf::Vector2i getPosition();
	void setSize(sf::Vector2f size);
	sf::Vector2i getSize();
	sf::Vector2i getContentPosition();
	sf::Vector2i getContentSize();
	virtual void setPosition(sf::Vector2i position);

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};

extern std::vector < Dialog* > dialogs;
