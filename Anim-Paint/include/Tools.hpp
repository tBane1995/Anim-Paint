#pragma once
#include "SFML/Graphics.hpp"
#include "Button.hpp"

class Separator {
public:
	sf::RectangleShape rect;

	Separator();
	~Separator();

	void setPosition(sf::Vector2f position);
	sf::Vector2f getSize();
	void draw();
};

class ColorButton : public NormalButton {
public:

	std::wstring name;
	sf::Color color;
	sf::RectangleShape rect;

	ColorButton(std::wstring color_name, sf::Color color);
	~ColorButton();

	void setPosition(sf::Vector2f position);
	void draw();
};

class LargeColorButton : public NormalButton {
public:

	sf::Color color;
	sf::RectangleShape color_rect;

	LargeColorButton(sf::Color color);
	~LargeColorButton();
	
	void setPosition(sf::Vector2f position);
	void setColor(sf::Color color);
	void draw();
};

enum class ToolType { Brush, Eraser, Selector, Lasso, Fill, Picker };

class Tools : public ElementGUI {
public:

	sf::RectangleShape rect;

	std::vector < Separator* > separators;

	std::vector < Button* > clipboard;
	sf::Text clipboard_text;
	NormalButton* btn_paste;
	ButtonWithTopTextAndList* btn_paste_2;
	ButtonWithRightText* btn_cut;
	ButtonWithRightText* btn_copy;
	ButtonWithBottomText* btn_select;
	ButtonWithBottomText* btn_lasso;
	
	std::vector < NormalButton*> tools;
	sf::Text tools_text;
	NormalButton* btn_brush;
	NormalButton* btn_picker;
	NormalButton* btn_fill;
	NormalButton* btn_eraser;

	std::vector < NormalButton* > sizes;
	sf::Text sizes_text;
	NormalButton* size_decrease;
	NormalButton* size_increase;

	LargeColorButton* first_color;
	LargeColorButton* second_color;
	sf::Text first_color_text_col;
	sf::Text first_color_text_val;
	sf::Text second_color_text_col;
	sf::Text second_color_text_val;
	sf::Text colors_text;
	std::vector < ColorButton* > colors;

	ToolType toolType;
	Button* selectedClipboardButton;
	Button* selectedToolButton;
	LargeColorButton* selectedColorButton;


	Tools();
	~Tools();

	void setPosition(sf::Vector2f position);
	void selectToolButton(Button* toolButton);
	void selectColorButton(LargeColorButton* colorButton);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

extern Tools* tools;


