#pragma once
#include "SFML/Graphics.hpp"
#include "ElementGUI/Button.hpp"

class Separator {
public:
	sf::IntRect _rect;
	Separator();
	~Separator();

	void setPosition(sf::Vector2i position);
	sf::Vector2i getSize();
	void draw();
};

class ColorButton : public NormalButton {
public:

	sf::Color _color;

	ColorButton(sf::Color color);
	~ColorButton();

	void setPosition(sf::Vector2i position);
	void draw();
};

class LargeColorButton : public NormalButton {
public:

	sf::Color _color;

	LargeColorButton(sf::Color color);
	~LargeColorButton();
	
	void setPosition(sf::Vector2i position);
	void setColor(sf::Color color);
	void draw();
};

class PaletteButton : public ButtonWithBottomText {
public:

	sf::Shader _shader;

	PaletteButton(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position);
	~PaletteButton();

	void draw();
};

enum class ToolType { Brush, Eraser, Selector, Lasso, Fill, Picker };

class Toolbar : public ElementGUI {
public:

	sf::IntRect _rect;

	std::vector<std::shared_ptr<Separator>> _separators;

	std::vector<std::shared_ptr<Button>> _clipboard;
	std::unique_ptr<sf::Text> _clipboard_text;
	std::shared_ptr<NormalButton> _btn_paste;
	std::shared_ptr<ButtonWithTopTextAndList> _btn_paste_2;
	std::shared_ptr<ButtonWithRightText> _btn_cut;
	std::shared_ptr<ButtonWithRightText> _btn_copy;
	std::shared_ptr<ButtonWithBottomText> _btn_select;
	std::shared_ptr<ButtonWithBottomText> _btn_lasso;
	
	std::vector<std::shared_ptr<NormalButton>> _tools;
	std::unique_ptr<sf::Text> _tools_text;
	std::shared_ptr<NormalButton> _btn_brush;
	std::shared_ptr<NormalButton> _btn_picker;
	std::shared_ptr<NormalButton> _btn_fill;
	std::shared_ptr<NormalButton> _btn_eraser;

	std::vector<std::shared_ptr<NormalButton>> _sizes;
	std::unique_ptr<sf::Text> _sizes_text;
	std::shared_ptr<NormalButton> _size_decrease;
	std::shared_ptr<NormalButton> _size_increase;

	std::shared_ptr<LargeColorButton> _first_color;
	std::shared_ptr<LargeColorButton> _second_color;
	std::unique_ptr<sf::Text> _first_color_text_col;
	std::unique_ptr<sf::Text> _first_color_text_val;
	std::unique_ptr<sf::Text> _second_color_text_col;
	std::unique_ptr<sf::Text> _second_color_text_val;
	std::unique_ptr<sf::Text> _colors_text;
	std::vector<std::shared_ptr<ColorButton>> _colors;
	std::shared_ptr<PaletteButton> _btn_palette_colors;

	ToolType _toolType;
	std::shared_ptr<Button> _selectedClipboardButton;
	std::shared_ptr<Button> _selectedToolButton;
	std::shared_ptr<LargeColorButton> _selectedColorButton;


	Toolbar();
	~Toolbar();

	sf::Vector2i getSize();
	void setPosition(sf::Vector2i position);
	void selectToolButton(std::shared_ptr<Button> toolButton);
	void selectColorButton(std::shared_ptr<LargeColorButton> colorButton);
	

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

extern std::shared_ptr<Toolbar> toolbar;


