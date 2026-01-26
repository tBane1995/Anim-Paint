#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "ElementGUI/Button.hpp"

enum class DialogState { Idle, ToClose };
enum class DialogClickArea { Inside, OutSide };

class Dialog : public ElementGUI {
public:
	sf::Vector2i _position;
	std::wstring _title;

	sf::IntRect _rect;
	sf::IntRect _titleRect;
	std::unique_ptr<sf::Text> _titleText;
	sf::IntRect _contentRect;
	std::shared_ptr<NormalButton> _closeBtn;

	DialogState _state;
	bool _absolutePositioning;

	// if cursor is inside dialog or outside
	DialogClickArea _clickArea;
	sf::Time _startFlashTime;

	

	bool _is_moved;
	sf::Vector2i _offset;

	std::vector<std::shared_ptr<ElementGUI>> _onTabElements;
	int _currentOnTabElement;

	Dialog();
	Dialog(std::wstring title, sf::Vector2i size, sf::Vector2i position = sf::Vector2i(-1, -1), bool absolutePositioning = false);
	virtual ~Dialog();

	sf::Vector2i getPosition();
	void setSize(sf::Vector2f size);
	sf::Vector2i getSize();
	sf::Vector2i getContentPosition();
	sf::Vector2i getContentSize();
	virtual void setPosition(sf::Vector2i position);
	void clampPosition();
	void deactivateOnTabElement();
	void activateOnTabElement(int id);

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};

extern std::vector<std::shared_ptr<Dialog>> dialogs;
