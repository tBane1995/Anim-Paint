#include "Tools/Toolbar.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Tools/Lasso.hpp"
#include "Animation/Animation.hpp"
#include "Tools/Selection.hpp"
#include "Tools/Brush.hpp"
#include "Mouse.hpp"
#include "Dialogs/Dialog.hpp"

Separator::Separator() {
	_rect = sf::RectangleShape(sf::Vector2f(2, tools_separator_height));
	_rect.setFillColor(tools_separator_color);
}

Separator::~Separator() {}

void Separator::setPosition(sf::Vector2f position) {
	_rect.setPosition(position + sf::Vector2f(0, (tools_height - tools_separator_height) / 2));
}

sf::Vector2f Separator::getSize() {
	return _rect.getSize();
}

void Separator::draw() {
	window->draw(_rect);
}

///////////////////////////////////////////////////////////////////////////

ColorButton::ColorButton(std::wstring color_name, sf::Color color) : NormalButton(getTexture(L"tex\\tools\\frame.png"), getTexture(L"tex\\tools\\frame_hover.png")) {
	_name = color_name;
	_color = color;

	_rect = sf::RectangleShape(sf::Vector2f(24, 24));
	_rect.setFillColor(color);
}

ColorButton::~ColorButton() {

}

void ColorButton::setPosition(sf::Vector2f position) {
	NormalButton::setPosition(position);
	_rect.setPosition(position + sf::Vector2f(4, 4));

}

void ColorButton::draw() {
	NormalButton::draw();
	window->draw(_rect);

}

/////////////////////////////////////////////////////////////////////////////

LargeColorButton::LargeColorButton(sf::Color color) : NormalButton(getTexture(L"tex\\tools\\frame_large.png"), getTexture(L"tex\\tools\\frame_large_hover.png")) {
	_color = color;

	_rect = sf::RectangleShape(sf::Vector2f(50 - 2 * tools_border_width, 50 - 2 * tools_border_width));
	_rect.setFillColor(sf::Color::Transparent);
	_rect.setOutlineThickness(tools_border_width);
	_rect.setOutlineColor(sf::Color::Transparent);

	_color_rect = sf::RectangleShape(sf::Vector2f(40, 40));
	_color_rect.setFillColor(_color);
}

LargeColorButton::~LargeColorButton() {

}

void LargeColorButton::setPosition(sf::Vector2f position) {
	NormalButton::setPosition(position);
	_color_rect.setPosition(_position + sf::Vector2f(4, 4));
	_rect.setPosition(_position);
}

void LargeColorButton::setColor(sf::Color color) {
	_color = color;
	_color_rect.setFillColor(_color);
}

void LargeColorButton::draw() {
	NormalButton::draw();
	window->draw(_color_rect);

}

/////////////////////////////////////////////////////////////////////////////

Toolbar::Toolbar() : 
	ElementGUI(),
	_clipboard_text(basicFont, L"clipboard", 13),
	_tools_text(basicFont, L"tools", 13),
	_sizes_text(basicFont, L"size", 13),
	_first_color_text_col(basicFont, L"color", 13),
	_first_color_text_val(basicFont, L"1", 13),
	_second_color_text_col(basicFont, L"color", 13),
	_second_color_text_val(basicFont, L"2", 13),
	_colors_text(basicFont, L"colors", 13)
{

	_rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, tools_height));
	_rect.setFillColor(tools_bar_color);

	// clipboard
	_clipboard_text.setFillColor(tools_text_color);

	_btn_paste = new NormalButton(getTexture(L"tex\\tools\\btn_paste.png"), getTexture(L"tex\\tools\\btn_paste_hover.png"));
	_btn_paste->_onclick_func = [this]() {
		_toolType = ToolType::Lasso;
		//selection->paste(&animation->getCurrentLayer()->image, selection->img);
		lasso->paste(&animation->getCurrentLayer()->_image, sf::Color::Transparent);
		};
	_btn_paste_2 = new ButtonWithTopTextAndList(L"paste", sf::Color::Transparent, tools_text_color, tools_text_hover_color);
	_btn_paste_2->_onclick_func = [this]() {

		};
	_btn_cut = new ButtonWithRightText(L"cut", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_cut.png"), getTexture(L"tex\\tools\\btn_cut_hover.png"));
	_btn_cut->_onclick_func = [this]() {
		//selection->cut(&animation->getCurrentLayer()->image, selection->img, second_color->color);
		lasso->cut(&animation->getCurrentLayer()->_image, _second_color->_color);
		};
	_btn_copy = new ButtonWithRightText(L"copy", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_copy.png"), getTexture(L"tex\\tools\\btn_copy_hover.png"));
	_btn_copy->_onclick_func = [this]() {
		//selection->copy(&animation->getCurrentLayer()->image, selection->img);
		lasso->copy(&animation->getCurrentLayer()->_image, _second_color->_color);

		};
	_btn_select = new ButtonWithBottomText(L"select", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_select.png"), getTexture(L"tex\\tools\\btn_select_hover.png"));
	_btn_select->_onclick_func = [this]() {
		_toolType = ToolType::Selector;
		selectToolButton(_btn_select);
		};
	_btn_lasso = new ButtonWithBottomText(L"lasso", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_lasso.png"), getTexture(L"tex\\tools\\btn_lasso_hover.png"));
	_btn_lasso->_onclick_func = [this]() {
		_toolType = ToolType::Lasso;
		selectToolButton(_btn_lasso);
		};

	_clipboard.clear();
	_clipboard.push_back(_btn_paste);
	_clipboard.push_back(_btn_paste_2);
	_clipboard.push_back(_btn_cut);
	_clipboard.push_back(_btn_copy);
	_clipboard.push_back(_btn_select);
	_clipboard.push_back(_btn_lasso);
	_separators.push_back(new Separator());

	// tools
	_tools_text.setFillColor(tools_text_color);

	_btn_brush = new NormalButton(getTexture(L"tex\\tools\\btn_brush.png"), getTexture(L"tex\\tools\\btn_brush_hover.png"));
	_btn_brush->_onclick_func = [this]() {
		_toolType = ToolType::Brush;
		brush->setBrushType(BrushType::Circle);
		selection->_state = SelectionState::None;
		selectToolButton(_btn_brush);
		};
	_btn_picker = new NormalButton(getTexture(L"tex\\tools\\btn_picker.png"), getTexture(L"tex\\tools\\btn_picker_hover.png"));
	_btn_picker->_onclick_func = [this]() {
		_toolType = ToolType::Picker;
		brush->setBrushType(BrushType::Circle);
		selection->_state = SelectionState::None;
		selectToolButton(_btn_picker);
		};
	_btn_fill = new NormalButton(getTexture(L"tex\\tools\\btn_fill.png"), getTexture(L"tex\\tools\\btn_fill_hover.png"));
	_btn_fill->_onclick_func = [this]() {
		_toolType = ToolType::Fill;
		selection->_state = SelectionState::None;
		selectToolButton(_btn_fill);
		};
	_btn_eraser = new NormalButton(getTexture(L"tex\\tools\\btn_eraser.png"), getTexture(L"tex\\tools\\btn_eraser_hover.png"));
	_btn_eraser->_onclick_func = [this]() {
		_toolType = ToolType::Eraser;
		brush->setBrushType(BrushType::Square);
		selection->_state = SelectionState::None;
		selectToolButton(_btn_eraser);
		};

	_tools.clear();
	_tools.push_back(_btn_brush);
	_tools.push_back(_btn_picker);
	_tools.push_back(_btn_fill);
	_tools.push_back(_btn_eraser);

	_toolType = ToolType::Brush;

	_separators.push_back(new Separator());

	// increase / decrease brush size
	_sizes_text.setFillColor(tools_text_color);

	_size_decrease = new NormalButton(getTexture(L"tex\\tools\\btn_size_decrease.png"), getTexture(L"tex\\tools\\btn_size_decrease_hover.png"));
	_size_decrease->_onclick_func = [this]() {
		if (_toolType == ToolType::Brush || _toolType == ToolType::Eraser)
			brush->decrease();
		};
	_size_increase = new NormalButton(getTexture(L"tex\\tools\\btn_size_increase.png"), getTexture(L"tex\\tools\\btn_size_increase_hover.png"));
	_size_increase->_onclick_func = [this]() {
		if (_toolType == ToolType::Brush || _toolType == ToolType::Eraser)
			brush->increase();
		};
	_sizes.clear();
	_sizes.push_back(_size_decrease);
	_sizes.push_back(_size_increase);

	_separators.push_back(new Separator());

	// main colors
	_first_color = new LargeColorButton(sf::Color::Black);
	_second_color = new LargeColorButton(sf::Color::White);
	_selectedColorButton = _first_color;

	_first_color_text_col.setFillColor(tools_text_color);
	_first_color_text_val.setFillColor(tools_text_color);
	_second_color_text_col.setFillColor(tools_text_color);
	_second_color_text_val.setFillColor(tools_text_color);

	_first_color->_onclick_func = [this]() {
		selectColorButton(_first_color);
		};

	_second_color->_onclick_func = [this]() {
		selectColorButton(_second_color);
		};

	_separators.push_back(new Separator());

	// colors
	_colors_text.setFillColor(tools_text_color);

	// black - grey - white
	_colors.push_back(new ColorButton(L"black", sf::Color(0, 0, 0)));
	_colors.push_back(new ColorButton(L"white", sf::Color(255, 255, 255)));

	// greys
	_colors.push_back(new ColorButton(L"dark grey", sf::Color(63, 63, 63)));
	_colors.push_back(new ColorButton(L"light grey", sf::Color(159, 159, 159)));

	// red
	_colors.push_back(new ColorButton(L"dark red", sf::Color(63, 0, 0)));
	_colors.push_back(new ColorButton(L"light red", sf::Color(191, 0, 0)));

	// orange
	_colors.push_back(new ColorButton(L"dark orange", sf::Color(127, 63, 0)));
	_colors.push_back(new ColorButton(L"light orange", sf::Color(255, 127, 0)));

	// yellow
	_colors.push_back(new ColorButton(L"dark yellow", sf::Color(63, 63, 0)));
	_colors.push_back(new ColorButton(L"light yellow", sf::Color(191, 191, 0)));

	// green
	_colors.push_back(new ColorButton(L"dark green", sf::Color(0, 63, 0)));
	_colors.push_back(new ColorButton(L"light green", sf::Color(0, 191, 0)));

	// cyan
	_colors.push_back(new ColorButton(L"dark cyan", sf::Color(0, 63, 63)));
	_colors.push_back(new ColorButton(L"light cyan", sf::Color(0, 191, 191)));

	// blue
	_colors.push_back(new ColorButton(L"dark blue", sf::Color(0, 0, 63)));
	_colors.push_back(new ColorButton(L"light blue", sf::Color(0, 0, 191)));

	// magenta
	_colors.push_back(new ColorButton(L"dark magenta", sf::Color(63, 0, 63)));
	_colors.push_back(new ColorButton(L"light magenta", sf::Color(191, 0, 191)));

	for (auto& c : _colors) {
		c->_onclick_func = [this, c]() {
			_selectedColorButton->setColor(c->_color);
			};
	}

	_selectedToolButton = nullptr;
	selectToolButton(_btn_brush);

	_selectedColorButton = nullptr;
	selectColorButton(_first_color);

	setPosition(sf::Vector2f(0, 0));
}

Toolbar::~Toolbar() {
	for (auto& sep : _separators)
		delete sep;

	for (auto& btn : _clipboard)
		delete btn;

	for (auto& btn : _tools)
		delete btn;

	for (auto& btn : _sizes)
		delete btn;

	for (auto& col : _colors)
		delete col;

	delete _first_color;
	delete _second_color;
}

void Toolbar::setPosition(sf::Vector2f position) {
	_rect.setPosition(position + sf::Vector2f(0, menu_height));

	position += sf::Vector2f(menuoptions_border_width, 0);

	// clipboard
	_clipboard[0]->setPosition(position + sf::Vector2f(0, menu_height));


	_clipboard[1]->setPosition(position + sf::Vector2f(0, menu_height + 32));
	int x = _clipboard[1]->getSize().x;

	_clipboard[2]->setPosition(position + sf::Vector2f(x, menu_height + 2));
	_clipboard[3]->setPosition(position + sf::Vector2f(x, menu_height + 32 + 2));
	x += _clipboard[3]->getSize().x;

	_clipboard[4]->setPosition(position + sf::Vector2f(x, menu_height));
	x += _clipboard[4]->getSize().x;

	_clipboard[5]->setPosition(position + sf::Vector2f(x, menu_height));
	x += _clipboard[5]->getSize().x;

	x += 4;

	_clipboard_text.setPosition(sf::Vector2f(x / 2 - _clipboard_text.getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	_separators[0]->setPosition(sf::Vector2f(x, menu_height));
	x += _separators[0]->getSize().x;

	// tools
	int old_x = x;
	int y = menu_height + 4;
	x += 2;
	for (int i = 0; i < _tools.size(); i += 2) {
		_tools[i]->setPosition(sf::Vector2f(x, y));
		_tools[i + 1]->setPosition(sf::Vector2f(x, y + 32));
		x += 32;
	}

	x += 2;

	_tools_text.setPosition(sf::Vector2f((x + old_x) / 2 - _tools_text.getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	_separators[1]->setPosition(sf::Vector2f(x, menu_height));
	x += _separators[1]->getSize().x;

	// sizes
	old_x = x;

	x += 2;
	_sizes[0]->setPosition(sf::Vector2f(x, menu_height + 4));
	_sizes[1]->setPosition(sf::Vector2f(x, menu_height + 32 + 4));
	x += 32;
	x += 2;

	_sizes_text.setPosition(sf::Vector2f((x + old_x) / 2 - _sizes_text.getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	_separators[2]->setPosition(sf::Vector2f(x, menu_height));
	x += _separators[2]->getSize().x;

	// main colors
	y = menu_height + 4;
	x += 4;

	_first_color->setPosition(sf::Vector2f(x, y));
	_first_color_text_col.setPosition(sf::Vector2f(x + 24 - _first_color_text_col.getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) * 2 - 4));
	_first_color_text_val.setPosition(sf::Vector2f(x + 24 - _first_color_text_val.getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));
	x += 48;


	_second_color->setPosition(sf::Vector2f(x, y));
	_second_color_text_col.setPosition(sf::Vector2f(x + 24 - _second_color_text_col.getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) * 2 - 4));
	_second_color_text_val.setPosition(sf::Vector2f(x + 24 - _second_color_text_val.getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	x += 48 + 4;

	_separators[3]->setPosition(sf::Vector2f(x, menu_height));
	x += _separators[3]->getSize().x;

	// colors
	y = menu_height + 4;
	x += 4;
	old_x = x;
	for (int i = 0; i < _colors.size(); i += 2) {
		_colors[i]->setPosition(sf::Vector2f(x, y));
		_colors[i + 1]->setPosition(sf::Vector2f(x, y + 32));
		x += 32;
	}

	_colors_text.setPosition(sf::Vector2f((x + old_x) / 2 - _colors_text.getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

}

void Toolbar::selectToolButton(Button* toolButton) {
	if (_selectedToolButton != nullptr) {
		_selectedToolButton->_rect.setFillColor(sf::Color::Transparent);
		_selectedToolButton->_rect.setOutlineColor(sf::Color::Transparent);
	}

	_selectedToolButton = toolButton;

	if (_selectedToolButton != nullptr) {
		_selectedToolButton->_rect.setFillColor(sf::Color(95, 47, 47));
		_selectedToolButton->_rect.setOutlineColor(sf::Color(127, 47, 47));
	}
}

void Toolbar::selectColorButton(LargeColorButton* colorButton) {
	if (_selectedColorButton != nullptr) {
		_selectedColorButton->_rect.setFillColor(sf::Color::Transparent);
		_selectedColorButton->_rect.setOutlineColor(sf::Color::Transparent);
	}

	_selectedColorButton = colorButton;

	if (_selectedColorButton != nullptr) {
		_selectedColorButton->_rect.setFillColor(sf::Color(95, 47, 47));
		_selectedColorButton->_rect.setOutlineColor(sf::Color(127, 47, 47));
	}
}

void Toolbar::cursorHover() {

	if (!dialogs.empty())
		return;

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
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

}

void Toolbar::handleEvent(const sf::Event& event) {

	if (!dialogs.empty())
		return;

	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
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

	if (_selectedToolButton != nullptr) {
		_selectedToolButton->_rect.setFillColor(tools_button_press_color);
		_selectedToolButton->_rect.setOutlineColor(tools_button_press_border_color);
	}

	if (_selectedColorButton != nullptr) {
		_selectedColorButton->_rect.setFillColor(tools_button_press_color);
		_selectedColorButton->_rect.setOutlineColor(tools_button_press_border_color);
	}

}

void Toolbar::draw() {
	window->draw(_rect);

	for (auto& separator : _separators) {
		separator->draw();
	}

	window->draw(_clipboard_text);
	window->draw(_colors_text);
	window->draw(_tools_text);
	window->draw(_sizes_text);

	for (auto& tool : _clipboard)
		tool->draw();

	for (auto& tool : _tools)
		tool->draw();

	for (auto size : _sizes) {
		size->draw();
	}

	_first_color->draw();
	_second_color->draw();
	window->draw(_first_color_text_col);
	window->draw(_first_color_text_val);
	window->draw(_second_color_text_col);
	window->draw(_second_color_text_val);

	for (auto& col : _colors)
		col->draw();


}

Toolbar* toolbar = nullptr;