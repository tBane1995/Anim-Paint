#pragma once
#include "Dialogs/Dialog.hpp"
#include "Controls/ColoredButtonWithText.hpp"

class ConfirmDialog : public Dialog {
public:

	std::wstring _textStr;
	std::unique_ptr<sf::Text> _text;

	const static int buttonPaddingHorizontal = 32;
	const static int butttonPaddingVertical = 8;

	std::shared_ptr<ColoredButtonWithText> _confirmBtn;
	std::shared_ptr<ColoredButtonWithText> _cancelBtn;



	ConfirmDialog(std::wstring title, std::wstring text, sf::Vector2i position = sf::Vector2i(-1, -1), bool absolutePositioning = false);
	virtual ~ConfirmDialog();

	void setPosition(sf::Vector2i position);

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};