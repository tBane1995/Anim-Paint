#include "Components/Toolbar.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Components/MainMenu.hpp"
#include "Tools/Lasso.hpp"
#include "Animation/Animation.hpp"
#include "Tools/Selection.hpp"
#include "Tools/Brush.hpp"
#include "Cursor.hpp"
#include "Dialogs/Dialog.hpp"
#include "Tools/Filters.hpp"
#include "Time.hpp"
#include "Dialogs/Palette.hpp"
#include "DebugLog.hpp"
#include "Tools/ClipBoard.hpp"

Separator::Separator() {
	_rect = sf::IntRect(sf::Vector2i(0,0), sf::Vector2i(2, tools_separator_height));

}

Separator::~Separator() {}

void Separator::setPosition(sf::Vector2i position) {
	_rect.position = (position + sf::Vector2i(0, (tools_height - tools_separator_height) / 2));
}

sf::Vector2i Separator::getSize() {
	return _rect.size;
}

void Separator::draw() {
	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setFillColor(tools_separator_color);
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);
}

///////////////////////////////////////////////////////////////////////////

ColorButton::ColorButton(std::wstring color_name, sf::Color color) : NormalButton(getTexture(L"tex\\tools\\frame.png"), getTexture(L"tex\\tools\\frame_hover.png")) {
	_name = color_name;
	_color = color;

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(32, 32));
}

ColorButton::~ColorButton() {

}

void ColorButton::setPosition(sf::Vector2i position) {
	NormalButton::setPosition(position);

}

void ColorButton::draw() {
	NormalButton::draw();
	sf::RectangleShape rect(sf::Vector2f(24,24));
	rect.setFillColor(_color);
	rect.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(4, 4));
	window->draw(rect);

}

/////////////////////////////////////////////////////////////////////////////

LargeColorButton::LargeColorButton(sf::Color color) : NormalButton(getTexture(L"tex\\tools\\frame_large.png"), getTexture(L"tex\\tools\\frame_large_hover.png")) {
	_color = color;
}

LargeColorButton::~LargeColorButton() {

}

void LargeColorButton::setPosition(sf::Vector2i position) {
	NormalButton::setPosition(position);
}

void LargeColorButton::setColor(sf::Color color) {
	_color = color;
}

void LargeColorButton::draw() {
	NormalButton::draw();
	//_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(50 - 2 * tools_border_width, 50 - 2 * tools_border_width))
	sf::RectangleShape color_rect(sf::Vector2f(40, 40));
	color_rect.setFillColor(_color);
	color_rect.setPosition(sf::Vector2f(_rect.position) + sf::Vector2f(4, 4));
	window->draw(color_rect);

}

PaletteButton::PaletteButton(std::wstring text, sf::Color rectColor, sf::Color textColor, sf::Color hoverTextColor, std::shared_ptr<Texture> texture, std::shared_ptr<Texture> hoverTexture, sf::Vector2i position = sf::Vector2i(0, 0)) : ButtonWithBottomText(text, rectColor, textColor, hoverTextColor, texture, hoverTexture, position) {
	_shader = sf::Shader();
	if (!_shader.loadFromMemory(palette_button_shader_source, sf::Shader::Type::Fragment)) {
		DebugError(L"PaletteButton::PaletteButton: failed to load shader from memory.");
		exit(0);
	}
}

PaletteButton::~PaletteButton() {

}

void PaletteButton::draw() {

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x - 2 * tools_border_width);
	rectSize.y = float(_rect.size.y - 2 * tools_border_width);

	sf::RectangleShape rect(rectSize);

	switch (_state) {
	case ButtonState::Pressed:
		rect.setFillColor(_rectPressColor);
		rect.setOutlineThickness((float)(_rectBorderWidth));
		rect.setOutlineColor(_rectPressBorderColor);
		break;
	case ButtonState::Hover:
		rect.setFillColor(_rectHoverColor);
		rect.setOutlineThickness((float)(_rectBorderWidth));
		rect.setOutlineColor(_rectHoverBorderColor);
		break;
	case ButtonState::Idle:
		if (_isSelected) {
			rect.setFillColor(_rectSelectColor);
			rect.setOutlineThickness((float)(_rectBorderWidth));
			rect.setOutlineColor(_rectSelectBorderColor);
		}
		else {
			rect.setFillColor(_rectIdleColor);
			rect.setOutlineThickness((float)(_rectBorderWidth));
			rect.setOutlineColor(_rectIdleBorderColor);
		};
		break;
	};

	sf::Vector2f rectPosition;
	rectPosition.x = float(_rect.position.x + _rectBorderWidth);
	rectPosition.y = float(_rect.position.y + _rectBorderWidth);
	rect.setPosition(rectPosition);

	window->draw(rect);
	
	sf::Sprite sprite((_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	sf::RenderStates rs;
	
	_shader.setUniform("iTime", float(currentTime.asSeconds()));

	const sf::Texture& tex = (_state == ButtonState::Idle) ? *_texture->_texture : *_hoverTexture->_texture;
	sf::IntRect r = sprite.getTextureRect();          // px w atlasie
	sf::Vector2u ts = tex.getSize();                  // rozmiar atlasu

	sf::Glsl::Vec4 texRectUV(
		float(r.position.x) / ts.x,
		float(r.position.y) / ts.y,
		float(r.position.x + r.size.x) / ts.x,
		float(r.position.y + r.size.y) / ts.y
	);
	
	_shader.setUniform("texRectUV", texRectUV);
	rs.shader = &_shader;
	window->draw(sprite, rs);

	window->draw(*_text);
}

/////////////////////////////////////////////////////////////////////////////

Toolbar::Toolbar() : ElementGUI() {

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(window->getSize().x, tools_height));

	// clipboard
	_btn_paste = std::make_shared<NormalButton>(getTexture(L"tex\\tools\\btn_paste.png"), getTexture(L"tex\\tools\\btn_paste_hover.png"));
	_btn_paste->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_paste->_onclick_func = [this]() {
		if (selection->paste(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent)) {
			_toolType = ToolType::Selector;
			selection->_state = SelectionState::Selected;
			selectToolButton(_btn_select);
		}
		};
	_btn_paste->setTooltip(L"Paste", L"Paste the selection from the clipboard onto the canvas");

	_btn_paste_2 = std::make_shared<ButtonWithTopTextAndList>(L"paste", tools_text_color, tools_text_hover_color);
	_btn_paste_2->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_paste_2->_onclick_func = [this]() {

		};

	_btn_cut = std::make_shared<ButtonWithRightText>(L"cut", tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_cut.png"), getTexture(L"tex\\tools\\btn_cut_hover.png"));
	_btn_cut->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_cut->_onclick_func = [this]() {
		//selection->cut(&animation->getCurrentLayer()->image, selection->img, second_color->color);
		if (selection->_state == SelectionState::Selected) {
			selection->cut(getCurrentAnimation()->getCurrentLayer()->_image, _second_color->_color);
		}
		};
	_btn_cut->setTooltip(L"Cut", L"Cut the selection from the canvas and place it on the clipboard");

	_btn_copy = std::make_shared<ButtonWithRightText>(L"copy", tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_copy.png"), getTexture(L"tex\\tools\\btn_copy_hover.png"));
	_btn_copy->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_copy->_onclick_func = [this]() {
		if (selection->_state == SelectionState::Selected) {
			selection->copy(getCurrentAnimation()->getCurrentLayer()->_image, _second_color->_color);
			removeImageWithMask(getCurrentAnimation()->getCurrentLayer()->_image, selection->_resizedRect, *selection->_resizedMaskImage, toolbar->_second_color->_color);
		}
		};
	_btn_copy->setTooltip(L"Copy", L"Copy the selection from the canvas to the clipboard");

	_btn_select = std::make_shared<ButtonWithBottomText>(L"select", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_select.png"), getTexture(L"tex\\tools\\btn_select_hover.png"));
	_btn_select->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_select->_onclick_func = [this]() {
		_toolType = ToolType::Selector;
		selectToolButton(_btn_select);
		selection->unselect();
		};
	_btn_select->setTooltip(L"Selection Tool", L"Select an area of the canvas to move, copy, cut, or apply filters");

	_btn_lasso = std::make_shared<ButtonWithBottomText>(L"lasso", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_lasso.png"), getTexture(L"tex\\tools\\btn_lasso_hover.png"));
	_btn_lasso->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_lasso->_onclick_func = [this]() {
		_toolType = ToolType::Lasso;
		selectToolButton(_btn_lasso);
		selection->unselect();
		};
	_btn_lasso->setTooltip(L"Lasso Tool", L"Select an area of the canvas with a freehand selection to move, copy, cut, or apply filters");

	_clipboard.clear();
	_clipboard.push_back(_btn_paste);
	_clipboard.push_back(_btn_paste_2);
	_clipboard.push_back(_btn_cut);
	_clipboard.push_back(_btn_copy);
	_clipboard.push_back(_btn_select);
	_clipboard.push_back(_btn_lasso);
	_separators.push_back(std::make_shared<Separator>());

	_clipboard_text = std::make_unique<sf::Text>(basicFont, L"clipboard", 13);
	_clipboard_text->setFillColor(tools_text_color);


	// tools
	_btn_brush = std::make_shared<NormalButton>(getTexture(L"tex\\tools\\btn_brush.png"), getTexture(L"tex\\tools\\btn_brush_hover.png"));
	_btn_brush->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_brush->_onclick_func = [this]() {
		_toolType = ToolType::Brush;
		brush->setBrushType(BrushType::Circle);
		selectToolButton(_btn_brush);
		};
	_btn_picker = std::make_shared<NormalButton>(getTexture(L"tex\\tools\\btn_picker.png"), getTexture(L"tex\\tools\\btn_picker_hover.png"));
	_btn_picker->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_picker->_onclick_func = [this]() {
		_toolType = ToolType::Picker;
		brush->setBrushType(BrushType::Circle);
		selectToolButton(_btn_picker);
		};
	_btn_fill = std::make_shared<NormalButton>(getTexture(L"tex\\tools\\btn_fill.png"), getTexture(L"tex\\tools\\btn_fill_hover.png"));
	_btn_fill->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_fill->_onclick_func = [this]() {
		_toolType = ToolType::Fill;
		selectToolButton(_btn_fill);
		};
	_btn_eraser = std::make_shared<NormalButton>(getTexture(L"tex\\tools\\btn_eraser.png"), getTexture(L"tex\\tools\\btn_eraser_hover.png"));
	_btn_eraser->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_eraser->_onclick_func = [this]() {
		_toolType = ToolType::Eraser;
		brush->setBrushType(BrushType::Square);
		selectToolButton(_btn_eraser);
		};

	_tools.clear();
	_tools.push_back(_btn_brush);
	_tools.push_back(_btn_picker);
	_tools.push_back(_btn_fill);
	_tools.push_back(_btn_eraser);

	_tools_text = std::make_unique<sf::Text>(basicFont, L"tools", 13);
	_tools_text->setFillColor(tools_text_color);

	_toolType = ToolType::Brush;

	_separators.push_back(std::make_shared<Separator>());

	// increase / decrease brush size
	_size_decrease = std::make_shared<NormalButton>(getTexture(L"tex\\tools\\btn_size_decrease.png"), getTexture(L"tex\\tools\\btn_size_decrease_hover.png"));
	_size_decrease->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_size_decrease->_onclick_func = [this]() {
		if (_toolType == ToolType::Brush || _toolType == ToolType::Eraser)
			brush->decrease();
		};
	_size_increase = std::make_shared<NormalButton>(getTexture(L"tex\\tools\\btn_size_increase.png"), getTexture(L"tex\\tools\\btn_size_increase_hover.png"));
	_size_increase->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_size_increase->_onclick_func = [this]() {
		if (_toolType == ToolType::Brush || _toolType == ToolType::Eraser)
			brush->increase();
		};
	_sizes.clear();
	_sizes.push_back(_size_decrease);
	_sizes.push_back(_size_increase);

	_sizes_text = std::make_unique<sf::Text>(basicFont, L"size", 13);
	_sizes_text->setFillColor(tools_text_color);

	_separators.push_back(std::make_shared<Separator>());

	// main colors
	_first_color = std::make_shared<LargeColorButton>(sf::Color::Black);
	_first_color->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_second_color = std::make_shared<LargeColorButton>(sf::Color::White);
	_second_color->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);

	_selectedColorButton = _first_color;
	_selectedColorButton->select();

	_first_color->_onclick_func = [this]() {
		selectColorButton(_first_color);
		};

	_second_color->_onclick_func = [this]() {
		selectColorButton(_second_color);
		};

	_first_color_text_col = std::make_unique<sf::Text>(basicFont, L"color", 13);
	_first_color_text_col->setFillColor(tools_text_color);
	_first_color_text_val = std::make_unique<sf::Text>(basicFont, L"1", 13);
	_first_color_text_val->setFillColor(tools_text_color);

	_second_color_text_col = std::make_unique<sf::Text>(basicFont, L"color", 13);
	_second_color_text_col->setFillColor(tools_text_color);
	_second_color_text_val = std::make_unique<sf::Text>(basicFont, L"2", 13);
	_second_color_text_val->setFillColor(tools_text_color);

	_separators.push_back(std::make_shared<Separator>());

	// colors
	
	// black - grey - white
	_colors.push_back(std::make_shared<ColorButton>(L"black", sf::Color(0, 0, 0)));
	_colors.push_back(std::make_shared<ColorButton>(L"white", sf::Color(255, 255, 255)));

	// greys
	_colors.push_back(std::make_shared<ColorButton>(L"dark grey", sf::Color(63, 63, 63)));
	_colors.push_back(std::make_shared<ColorButton>(L"light grey", sf::Color(159, 159, 159)));

	// red
	_colors.push_back(std::make_shared<ColorButton>(L"dark red", sf::Color(63, 0, 0)));
	_colors.push_back(std::make_shared<ColorButton>(L"light red", sf::Color(191, 0, 0)));

	// orange
	_colors.push_back(std::make_shared<ColorButton>(L"dark orange", sf::Color(127, 63, 0)));
	_colors.push_back(std::make_shared<ColorButton>(L"light orange", sf::Color(255, 127, 0)));

	// yellow
	_colors.push_back(std::make_shared<ColorButton>(L"dark yellow", sf::Color(63, 63, 0)));
	_colors.push_back(std::make_shared<ColorButton>(L"light yellow", sf::Color(191, 191, 0)));

	// green
	_colors.push_back(std::make_shared<ColorButton>(L"dark green", sf::Color(0, 63, 0)));
	_colors.push_back(std::make_shared<ColorButton>(L"light green", sf::Color(0, 191, 0)));

	// cyan
	_colors.push_back(std::make_shared<ColorButton>(L"dark cyan", sf::Color(0, 63, 63)));
	_colors.push_back(std::make_shared<ColorButton>(L"light cyan", sf::Color(0, 191, 191)));

	// blue
	_colors.push_back(std::make_shared<ColorButton>(L"dark blue", sf::Color(0, 0, 63)));
	_colors.push_back(std::make_shared<ColorButton>(L"light blue", sf::Color(0, 0, 191)));

	// magenta
	_colors.push_back(std::make_shared<ColorButton>(L"dark magenta", sf::Color(63, 0, 63)));
	_colors.push_back(std::make_shared<ColorButton>(L"light magenta", sf::Color(191, 0, 191)));

	for (auto& c : _colors) {
		c->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
			tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
		c->_onclick_func = [this, c]() {
			_selectedColorButton->setColor(c->_color);
			if (palette != nullptr) {
				palette->setColorInRGBInputs(_selectedColorButton->_color);
				palette->loadColorFromRGBInputs();
			}
			};
	}

	_colors_text = std::make_unique<sf::Text>(basicFont, L"colors", 13);
	_colors_text->setFillColor(tools_text_color);

	_btn_palette_colors = std::make_shared<PaletteButton>(L"palette", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_palette_colors.png"), getTexture(L"tex\\tools\\btn_palette_colors_hover.png"));
	_btn_palette_colors->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color);
	_btn_palette_colors->_onclick_func = [this]() {
		if(palette == nullptr)
			palette = std::make_shared<Palette>();
		};

	//////////////////////////////////////////////////////////////////////////////

	_selectedToolButton = nullptr;
	selectToolButton(_btn_brush);

	_selectedColorButton = nullptr;
	selectColorButton(_first_color);

	setPosition(sf::Vector2i(0, menu_height));
}

Toolbar::~Toolbar() {

}

sf::Vector2i Toolbar::getSize() {
	return _rect.size;
}

void Toolbar::setPosition(sf::Vector2i position) {
	_rect.position = position;

	position = sf::Vector2i(_rect.position) + sf::Vector2i(menuoptions_border_width, 0);

	// clipboard 
	_clipboard[0]->setPosition(position + sf::Vector2i(0, 0));
	_clipboard[1]->setPosition(position + sf::Vector2i(0, 32));
	int x = _clipboard[1]->getSize().x;

	_clipboard[2]->setPosition(position + sf::Vector2i(x, 0));
	_clipboard[3]->setPosition(position + sf::Vector2i(x, 32));
	x += _clipboard[3]->getSize().x;

	_clipboard[4]->setPosition(position + sf::Vector2i(x, 0));
	x += _clipboard[4]->getSize().x;

	_clipboard[5]->setPosition(position + sf::Vector2i(x, 0));
	x += _clipboard[5]->getSize().x;
	x += 4;

	_clipboard_text->setPosition(sf::Vector2f(x / 2 - _clipboard_text->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	// first separator
	_separators[0]->setPosition(sf::Vector2i(x, menu_height));
	x += _separators[0]->getSize().x;

	// tools
	int old_x = x;
	int y = menu_height + 4;
	x += 2;
	for (int i = 0; i < _tools.size(); i += 2) {
		_tools[i]->setPosition(sf::Vector2i(x, y));
		_tools[i + 1]->setPosition(sf::Vector2i(x, y + 32));
		x += 32;
	}

	x += 2;

	_tools_text->setPosition(sf::Vector2f((x + old_x) / 2 - _tools_text->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	// second separator
	_separators[1]->setPosition(sf::Vector2i(x, menu_height));
	x += _separators[1]->getSize().x;

	// sizes
	old_x = x;
	x += 2;
	_sizes[0]->setPosition(sf::Vector2i(x, menu_height + 4));
	_sizes[1]->setPosition(sf::Vector2i(x, menu_height + 32 + 4));
	x += 32;
	x += 2;

	_sizes_text->setPosition(sf::Vector2f((x + old_x) / 2 - _sizes_text->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	_separators[2]->setPosition(sf::Vector2i(x, menu_height));
	x += _separators[2]->getSize().x;

	// first main color
	y = menu_height + 4;
	x += 4;

	_first_color->setPosition(sf::Vector2i(x, y));


	_first_color_text_col->setPosition(sf::Vector2f(x + 24 - _first_color_text_col->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) * 2 - 4));
	_first_color_text_val->setPosition(sf::Vector2f(x + 24 - _first_color_text_val->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));
	x += 48;


	// second main color
	_second_color->setPosition(sf::Vector2i(x, y));

	_second_color_text_col->setPosition(sf::Vector2f(x + 24 - _second_color_text_col->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) * 2 - 4));
	_second_color_text_val->setPosition(sf::Vector2f(x + 24 - _second_color_text_val->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	x += 48 + 4;

	// third separator
	_separators[3]->setPosition(sf::Vector2i(x, menu_height));
	x += _separators[3]->getSize().x;

	// colors
	y = menu_height + 4;
	x += 4;
	old_x = x;
	for (int i = 0; i < _colors.size(); i += 2) {
		_colors[i]->setPosition(sf::Vector2i(x, y));
		_colors[i + 1]->setPosition(sf::Vector2i(x, y + 32));
		x += 32;
	}

	_colors_text->setPosition(sf::Vector2f((x + old_x) / 2 - _colors_text->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	_btn_palette_colors->setPosition(position + sf::Vector2i(x, 0));

}

void Toolbar::selectToolButton(std::shared_ptr<Button> toolButton) {
	if (_selectedToolButton != nullptr) {
		_selectedToolButton->unselect();
	}

	_selectedToolButton = toolButton;

	if (_selectedToolButton != nullptr) {
		_selectedToolButton->select();
	}
}

void Toolbar::selectColorButton(std::shared_ptr<LargeColorButton> colorButton) {
	if (_selectedColorButton != nullptr) {
		_selectedColorButton->unselect();
	}

	_selectedColorButton = colorButton;

	if (_selectedColorButton != nullptr) {
		_selectedColorButton->select();
	}
}

void Toolbar::cursorHover() {

	if (!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (selection->_state == SelectionState::Selecting)
		return;

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}

	for (auto& tool : _clipboard)
		tool->cursorHover();

	for (auto& tool : _tools)
		tool->cursorHover();

	for (auto& tool : _sizes)
		tool->cursorHover();

	_first_color->cursorHover();
	_second_color->cursorHover();

	for (auto& color : _colors)
		color->cursorHover();

	_btn_palette_colors->cursorHover();
}

void Toolbar::handleEvent(const sf::Event& event) {

	if (!dialogs.empty()) {
		return;
	}
		
	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (_rect.contains(cursor->_worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
	}

	if (const auto* kp = event.getIf<sf::Event::KeyPressed>()) {

		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && kp->code == sf::Keyboard::Key::X) {
			_btn_cut->click();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && kp->code == sf::Keyboard::Key::C) {
			_btn_copy->click();
		}
		else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && kp->code == sf::Keyboard::Key::V) {
			_btn_paste->click();
		}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && kp->code == sf::Keyboard::Key::A) {
			_toolType = ToolType::Lasso;
			selectToolButton(_btn_lasso);
			selection->selectAll();
		}

	}

	for (auto& tool : _clipboard)
		tool->handleEvent(event);

	for (auto& tool : _tools)
		tool->handleEvent(event);

	for (auto& tool : _sizes)
		tool->handleEvent(event);

	_first_color->handleEvent(event);
	_second_color->handleEvent(event);

	for (auto& color : _colors)
		color->handleEvent(event);

	_btn_palette_colors->handleEvent(event);
}

void Toolbar::update() {
	for (auto& tool : _clipboard)
		tool->update();

	for (auto& tool : _tools)
		tool->update();

	for (auto& tool : _sizes)
		tool->update();

	_first_color->update();
	_second_color->update();

	for (auto& color : _colors)
		color->update();

	_btn_palette_colors->update();

}

void Toolbar::draw() {

	// main rect

	sf::Vector2f rectSize;
	rectSize.x = float(window->getSize().x);
	rectSize.y = float(tools_height);

	sf::RectangleShape rect(rectSize);
	rect.setFillColor(tools_bar_color);
	rect.setPosition(sf::Vector2f(_rect.position));

	
	// draw all elements
	window->draw(rect);
	window->draw(*_clipboard_text);
	window->draw(*_tools_text);
	window->draw(*_sizes_text);
	window->draw(*_first_color_text_col);
	window->draw(*_first_color_text_val);
	window->draw(*_second_color_text_col);
	window->draw(*_second_color_text_val);
	window->draw(*_colors_text);
	
	for (auto& tool : _tools) tool->draw();
	for (auto size : _sizes) size->draw();
	_first_color->draw();
	_second_color->draw();
	for (auto& col : _colors) col->draw();
	_btn_palette_colors->draw();
	for (auto& separator : _separators) separator->draw();
	for (auto& tool : _clipboard) tool->draw();

	
}

std::shared_ptr<Toolbar> toolbar;