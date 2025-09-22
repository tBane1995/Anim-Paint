#pragma once
#include "SFML/Graphics.hpp"
#include "ElementGUI/Button.hpp"

class Separator {
public:
	sf::RectangleShape _rect;

	Separator();
	~Separator();

	void setPosition(sf::Vector2f position);
	sf::Vector2f getSize();
	void draw();
};

class ColorButton : public NormalButton {
public:

	std::wstring _name;
	sf::Color _color;
	sf::RectangleShape _rect;

	ColorButton(std::wstring color_name, sf::Color color);
	~ColorButton();

	void setPosition(sf::Vector2f position);
	void draw();
};

class LargeColorButton : public NormalButton {
public:

	sf::Color _color;
	sf::RectangleShape _color_rect;

	LargeColorButton(sf::Color color);
	~LargeColorButton();
	
	void setPosition(sf::Vector2f position);
	void setColor(sf::Color color);
	void draw();
};

enum class ToolType { Brush, Eraser, Selector, Lasso, Fill, Picker };

class Toolbar : public ElementGUI {
public:

	sf::RectangleShape _rect;

	std::vector < Separator* > _separators;

	std::vector < Button* > _clipboard;
	sf::Text _clipboard_text;
	NormalButton* _btn_paste;
	ButtonWithTopTextAndList* _btn_paste_2;
	ButtonWithRightText* _btn_cut;
	ButtonWithRightText* _btn_copy;
	ButtonWithBottomText* _btn_select;
	ButtonWithBottomText* _btn_lasso;
	
	std::vector < NormalButton*> _tools;
	sf::Text _tools_text;
	NormalButton* _btn_brush;
	NormalButton* _btn_picker;
	NormalButton* _btn_fill;
	NormalButton* _btn_eraser;

	std::vector < NormalButton* > _sizes;
	sf::Text _sizes_text;
	NormalButton* _size_decrease;
	NormalButton* _size_increase;

	LargeColorButton* _first_color;
	LargeColorButton* _second_color;
	sf::Text _first_color_text_col;
	sf::Text _first_color_text_val;
	sf::Text _second_color_text_col;
	sf::Text _second_color_text_val;
	sf::Text _colors_text;
	std::vector < ColorButton* > _colors;

	ToolType _toolType;
	Button* _selectedClipboardButton;
	Button* _selectedToolButton;
	LargeColorButton* _selectedColorButton;


	Toolbar();
	~Toolbar();

	void setPosition(sf::Vector2f position);
	void selectToolButton(Button* toolButton);
	void selectColorButton(LargeColorButton* colorButton);

	void cursorHover();
	void handleEvent(sf::Event& event);
	void update();
	void draw();
};

extern Toolbar* toolbar;


