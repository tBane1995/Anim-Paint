#include "Tools/Toolbar.hpp"
#include "Theme.hpp"
#include "Window.hpp"
#include "Tools/Lasso.hpp"
#include "Animation/Animation.hpp"
#include "Tools/Selection.hpp"
#include "Tools/Brush.hpp"
#include "Mouse.hpp"

Separator::Separator() {
	rect = sf::RectangleShape(sf::Vector2f(2, tools_separator_height));
	rect.setFillColor(tools_separator_color);
}

Separator::~Separator() {}

void Separator::setPosition(sf::Vector2f position) {
	rect.setPosition(position + sf::Vector2f(0, (tools_height - tools_separator_height) / 2));
}

sf::Vector2f Separator::getSize() {
	return rect.getSize();
}

void Separator::draw() {
	window->draw(rect);
}

///////////////////////////////////////////////////////////////////////////

ColorButton::ColorButton(std::wstring color_name, sf::Color color) : NormalButton(getTexture(L"tex\\tools\\frame.png"), getTexture(L"tex\\tools\\frame_hover.png")) {
	this->name = color_name;
	this->color = color;

	rect = sf::RectangleShape(sf::Vector2f(24, 24));
	rect.setFillColor(color);
}

ColorButton::~ColorButton() {

}

void ColorButton::setPosition(sf::Vector2f position) {
	NormalButton::setPosition(position);
	rect.setPosition(position + sf::Vector2f(4, 4));

}

void ColorButton::draw() {
	NormalButton::draw();
	window->draw(rect);

}

/////////////////////////////////////////////////////////////////////////////

LargeColorButton::LargeColorButton(sf::Color color) : NormalButton(getTexture(L"tex\\tools\\frame_large.png"), getTexture(L"tex\\tools\\frame_large_hover.png")) {
	this->color = color;

	rect = sf::RectangleShape(sf::Vector2f(50 - 2 * tools_border_width, 50 - 2 * tools_border_width));
	rect.setFillColor(sf::Color::Transparent);
	rect.setOutlineThickness(tools_border_width);
	rect.setOutlineColor(sf::Color::Transparent);

	color_rect = sf::RectangleShape(sf::Vector2f(40, 40));
	color_rect.setFillColor(this->color);
}

LargeColorButton::~LargeColorButton() {

}

void LargeColorButton::setPosition(sf::Vector2f position) {
	NormalButton::setPosition(position);
	color_rect.setPosition(position + sf::Vector2f(4, 4));
	rect.setPosition(position);
}

void LargeColorButton::setColor(sf::Color color) {
	this->color = color;
	color_rect.setFillColor(this->color);
}

void LargeColorButton::draw() {
	NormalButton::draw();
	window->draw(color_rect);

}

/////////////////////////////////////////////////////////////////////////////

Toolbar::Toolbar() : ElementGUI() {

	rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, tools_height));
	rect.setFillColor(tools_bar_color);



	// clipboard
	clipboard_text = sf::Text(L"clipboard", basicFont, 13);
	clipboard_text.setFillColor(tools_text_color);

	btn_paste = new NormalButton(getTexture(L"tex\\tools\\btn_paste.png"), getTexture(L"tex\\tools\\btn_paste_hover.png"));
	btn_paste->onclick_func = [this]() {
		toolType = ToolType::Lasso;
		//selection->paste(&animation->getCurrentLayer()->image, selection->img);
		lasso->paste(&animation->getCurrentLayer()->image, sf::Color::Transparent);
		};
	btn_paste_2 = new ButtonWithTopTextAndList(L"paste", sf::Color::Transparent, tools_text_color, tools_text_hover_color);
	btn_paste_2->onclick_func = [this]() {

		};
	btn_cut = new ButtonWithRightText(L"cut", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_cut.png"), getTexture(L"tex\\tools\\btn_cut_hover.png"));
	btn_cut->onclick_func = [this]() {
		//selection->cut(&animation->getCurrentLayer()->image, selection->img, second_color->color);
		lasso->cut(&animation->getCurrentLayer()->image, second_color->color);
		};
	btn_copy = new ButtonWithRightText(L"copy", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_copy.png"), getTexture(L"tex\\tools\\btn_copy_hover.png"));
	btn_copy->onclick_func = [this]() {
		//selection->copy(&animation->getCurrentLayer()->image, selection->img);
		lasso->copy(&animation->getCurrentLayer()->image, second_color->color);

		};
	btn_select = new ButtonWithBottomText(L"select", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_select.png"), getTexture(L"tex\\tools\\btn_select_hover.png"));
	btn_select->onclick_func = [this]() {
		toolType = ToolType::Selector;
		selectToolButton(btn_select);
		};
	btn_lasso = new ButtonWithBottomText(L"lasso", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_lasso.png"), getTexture(L"tex\\tools\\btn_lasso_hover.png"));
	btn_lasso->onclick_func = [this]() {
		toolType = ToolType::Lasso;
		selectToolButton(btn_lasso);
		};

	clipboard.clear();
	clipboard.push_back(btn_paste);
	clipboard.push_back(btn_paste_2);
	clipboard.push_back(btn_cut);
	clipboard.push_back(btn_copy);
	clipboard.push_back(btn_select);
	clipboard.push_back(btn_lasso);
	separators.push_back(new Separator());

	// tools
	tools_text = sf::Text(L"tools", basicFont, 13);
	tools_text.setFillColor(tools_text_color);

	btn_brush = new NormalButton(getTexture(L"tex\\tools\\btn_brush.png"), getTexture(L"tex\\tools\\btn_brush_hover.png"));
	btn_brush->onclick_func = [this]() {
		toolType = ToolType::Brush;
		brush->setBrushType(BrushType::Circle);
		selection->state = SelectionState::None;
		selectToolButton(btn_brush);
		};
	btn_picker = new NormalButton(getTexture(L"tex\\tools\\btn_picker.png"), getTexture(L"tex\\tools\\btn_picker_hover.png"));
	btn_picker->onclick_func = [this]() {
		toolType = ToolType::Picker;
		brush->setBrushType(BrushType::Circle);
		selection->state = SelectionState::None;
		selectToolButton(btn_picker);
		};
	btn_fill = new NormalButton(getTexture(L"tex\\tools\\btn_fill.png"), getTexture(L"tex\\tools\\btn_fill_hover.png"));
	btn_fill->onclick_func = [this]() {
		toolType = ToolType::Fill;
		selection->state = SelectionState::None;
		selectToolButton(btn_fill);
		};
	btn_eraser = new NormalButton(getTexture(L"tex\\tools\\btn_eraser.png"), getTexture(L"tex\\tools\\btn_eraser_hover.png"));
	btn_eraser->onclick_func = [this]() {
		toolType = ToolType::Eraser;
		brush->setBrushType(BrushType::Square);
		selection->state = SelectionState::None;
		selectToolButton(btn_eraser);
		};

	tools.clear();
	tools.push_back(btn_brush);
	tools.push_back(btn_picker);
	tools.push_back(btn_fill);
	tools.push_back(btn_eraser);

	toolType = ToolType::Brush;

	separators.push_back(new Separator());

	// increase / decrease brush size
	sizes_text = sf::Text(L"size", basicFont, 13);
	sizes_text.setFillColor(tools_text_color);

	size_decrease = new NormalButton(getTexture(L"tex\\tools\\btn_size_decrease.png"), getTexture(L"tex\\tools\\btn_size_decrease_hover.png"));
	size_decrease->onclick_func = [this]() {
		if (toolType == ToolType::Brush || toolType == ToolType::Eraser)
			brush->decrease();
		};
	size_increase = new NormalButton(getTexture(L"tex\\tools\\btn_size_increase.png"), getTexture(L"tex\\tools\\btn_size_increase_hover.png"));
	size_increase->onclick_func = [this]() {
		if (toolType == ToolType::Brush || toolType == ToolType::Eraser)
			brush->increase();
		};
	sizes.clear();
	sizes.push_back(size_decrease);
	sizes.push_back(size_increase);

	separators.push_back(new Separator());

	// main colors
	first_color = new LargeColorButton(sf::Color::Black);
	second_color = new LargeColorButton(sf::Color::White);
	selectedColorButton = first_color;

	first_color_text_col = sf::Text(L"color", basicFont, 13);
	first_color_text_val = sf::Text(L"1", basicFont, 13);

	second_color_text_col = sf::Text(L"color", basicFont, 13);
	second_color_text_val = sf::Text(L"2", basicFont, 13);

	first_color_text_col.setFillColor(tools_text_color);
	first_color_text_val.setFillColor(tools_text_color);
	second_color_text_col.setFillColor(tools_text_color);
	second_color_text_val.setFillColor(tools_text_color);

	first_color->onclick_func = [this]() {
		selectColorButton(first_color);
		};

	second_color->onclick_func = [this]() {
		selectColorButton(second_color);
		};

	separators.push_back(new Separator());

	// colors
	colors_text = sf::Text(L"colors", basicFont, 13);
	colors_text.setFillColor(tools_text_color);

	// black - grey - white
	colors.push_back(new ColorButton(L"black", sf::Color(0, 0, 0)));
	colors.push_back(new ColorButton(L"white", sf::Color(255, 255, 255)));

	// greys
	colors.push_back(new ColorButton(L"dark grey", sf::Color(63, 63, 63)));
	colors.push_back(new ColorButton(L"light grey", sf::Color(159, 159, 159)));

	// red
	colors.push_back(new ColorButton(L"dark red", sf::Color(63, 0, 0)));
	colors.push_back(new ColorButton(L"light red", sf::Color(191, 0, 0)));

	// orange
	colors.push_back(new ColorButton(L"dark orange", sf::Color(127, 63, 0)));
	colors.push_back(new ColorButton(L"light orange", sf::Color(255, 127, 0)));

	// yellow
	colors.push_back(new ColorButton(L"dark yellow", sf::Color(63, 63, 0)));
	colors.push_back(new ColorButton(L"light yellow", sf::Color(191, 191, 0)));

	// green
	colors.push_back(new ColorButton(L"dark green", sf::Color(0, 63, 0)));
	colors.push_back(new ColorButton(L"light green", sf::Color(0, 191, 0)));

	// cyan
	colors.push_back(new ColorButton(L"dark cyan", sf::Color(0, 63, 63)));
	colors.push_back(new ColorButton(L"light cyan", sf::Color(0, 191, 191)));

	// blue
	colors.push_back(new ColorButton(L"dark blue", sf::Color(0, 0, 63)));
	colors.push_back(new ColorButton(L"light blue", sf::Color(0, 0, 191)));

	// magenta
	colors.push_back(new ColorButton(L"dark magenta", sf::Color(63, 0, 63)));
	colors.push_back(new ColorButton(L"light magenta", sf::Color(191, 0, 191)));

	for (auto& c : colors) {
		c->onclick_func = [this, c]() {
			selectedColorButton->setColor(c->color);
			};
	}

	selectedToolButton = nullptr;
	selectToolButton(btn_brush);

	selectedColorButton = nullptr;
	selectColorButton(this->first_color);

	setPosition(sf::Vector2f(0, 0));
}

Toolbar::~Toolbar() {
	for (auto& sep : separators)
		delete sep;

	for (auto& btn : clipboard)
		delete btn;

	for (auto& btn : tools)
		delete btn;

	for (auto& btn : sizes)
		delete btn;

	for (auto& col : colors)
		delete col;

	delete first_color;
	delete second_color;
}

void Toolbar::setPosition(sf::Vector2f position) {
	rect.setPosition(position + sf::Vector2f(0, menu_height));

	position += sf::Vector2f(menuoptions_border_width, 0);

	// clipboard
	clipboard[0]->setPosition(position + sf::Vector2f(0, menu_height));


	clipboard[1]->setPosition(position + sf::Vector2f(0, menu_height + 32));
	int x = clipboard[1]->getSize().x;

	clipboard[2]->setPosition(position + sf::Vector2f(x, menu_height + 2));
	clipboard[3]->setPosition(position + sf::Vector2f(x, menu_height + 32 + 2));
	x += clipboard[3]->getSize().x;

	clipboard[4]->setPosition(position + sf::Vector2f(x, menu_height));
	x += clipboard[4]->getSize().x;

	clipboard[5]->setPosition(position + sf::Vector2f(x, menu_height));
	x += clipboard[5]->getSize().x;

	x += 4;

	clipboard_text.setPosition(x / 2 - clipboard_text.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);

	separators[0]->setPosition(sf::Vector2f(x, menu_height));
	x += separators[0]->getSize().x;

	// tools
	int old_x = x;
	int y = menu_height + 4;
	x += 2;
	for (int i = 0; i < tools.size(); i += 2) {
		tools[i]->setPosition(sf::Vector2f(x, y));
		tools[i + 1]->setPosition(sf::Vector2f(x, y + 32));
		x += 32;
	}

	x += 2;

	tools_text.setPosition((x + old_x) / 2 - tools_text.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);

	separators[1]->setPosition(sf::Vector2f(x, menu_height));
	x += separators[1]->getSize().x;

	// sizes
	old_x = x;

	x += 2;
	sizes[0]->setPosition(sf::Vector2f(x, menu_height + 4));
	sizes[1]->setPosition(sf::Vector2f(x, menu_height + 32 + 4));
	x += 32;
	x += 2;

	sizes_text.setPosition((x + old_x) / 2 - sizes_text.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);

	separators[2]->setPosition(sf::Vector2f(x, menu_height));
	x += separators[2]->getSize().x;

	// main colors
	y = menu_height + 4;
	x += 4;

	first_color->setPosition(sf::Vector2f(x, y));
	first_color_text_col.setPosition(x + 24 - first_color_text_col.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) * 2 - 4);
	first_color_text_val.setPosition(x + 24 - first_color_text_val.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);
	x += 48;


	second_color->setPosition(sf::Vector2f(x, y));
	second_color_text_col.setPosition(x + 24 - second_color_text_col.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) * 2 - 4);
	second_color_text_val.setPosition(x + 24 - second_color_text_val.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);

	x += 48 + 4;

	separators[3]->setPosition(sf::Vector2f(x, menu_height));
	x += separators[3]->getSize().x;

	// colors
	y = menu_height + 4;
	x += 4;
	old_x = x;
	for (int i = 0; i < colors.size(); i += 2) {
		colors[i]->setPosition(sf::Vector2f(x, y));
		colors[i + 1]->setPosition(sf::Vector2f(x, y + 32));
		x += 32;
	}

	colors_text.setPosition((x + old_x) / 2 - colors_text.getGlobalBounds().width / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4);

}

void Toolbar::selectToolButton(Button* toolButton) {
	if (selectedToolButton != nullptr) {
		selectedToolButton->rect.setFillColor(sf::Color::Transparent);
		selectedToolButton->rect.setOutlineColor(sf::Color::Transparent);
	}

	selectedToolButton = toolButton;

	if (selectedToolButton != nullptr) {
		selectedToolButton->rect.setFillColor(sf::Color(95, 47, 47));
		selectedToolButton->rect.setOutlineColor(sf::Color(127, 47, 47));
	}
}

void Toolbar::selectColorButton(LargeColorButton* colorButton) {
	if (selectedColorButton != nullptr) {
		selectedColorButton->rect.setFillColor(sf::Color::Transparent);
		selectedColorButton->rect.setOutlineColor(sf::Color::Transparent);
	}

	selectedColorButton = colorButton;

	if (selectedColorButton != nullptr) {
		selectedColorButton->rect.setFillColor(sf::Color(95, 47, 47));
		selectedColorButton->rect.setOutlineColor(sf::Color(127, 47, 47));
	}
}

void Toolbar::cursorHover() {
	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	for (auto& tool : clipboard)
		tool->cursorHover();

	for (auto& tool : tools)
		tool->cursorHover();

	for (auto& tool : sizes)
		tool->cursorHover();

	first_color->cursorHover();
	second_color->cursorHover();

	for (auto& color : colors)
		color->cursorHover();

}

void Toolbar::handleEvent(sf::Event& event) {
	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			ElementGUI_pressed = this;
		}
	}

	for (auto& tool : clipboard)
		tool->handleEvent(event);

	for (auto& tool : tools)
		tool->handleEvent(event);

	for (auto& tool : sizes)
		tool->handleEvent(event);

	first_color->handleEvent(event);
	second_color->handleEvent(event);

	for (auto& color : colors)
		color->handleEvent(event);
}

void Toolbar::update() {
	for (auto& tool : clipboard)
		tool->update();

	for (auto& tool : tools)
		tool->update();

	for (auto& tool : sizes)
		tool->update();

	first_color->update();
	second_color->update();

	for (auto& color : colors)
		color->update();

	if (selectedToolButton != nullptr) {
		selectedToolButton->rect.setFillColor(tools_button_press_color);
		selectedToolButton->rect.setOutlineColor(tools_button_press_border_color);
	}

	if (selectedColorButton != nullptr) {
		selectedColorButton->rect.setFillColor(tools_button_press_color);
		selectedColorButton->rect.setOutlineColor(tools_button_press_border_color);
	}

}

void Toolbar::draw() {
	window->draw(rect);

	for (auto& separator : separators) {
		separator->draw();
	}

	window->draw(clipboard_text);
	window->draw(colors_text);
	window->draw(tools_text);
	window->draw(sizes_text);

	for (auto& tool : clipboard)
		tool->draw();

	for (auto& tool : tools)
		tool->draw();

	for (auto size : sizes) {
		size->draw();
	}

	first_color->draw();
	second_color->draw();
	window->draw(first_color_text_col);
	window->draw(first_color_text_val);
	window->draw(second_color_text_col);
	window->draw(second_color_text_val);

	for (auto& col : colors)
		col->draw();


}

Toolbar* toolbar = nullptr;