#include "Components/Toolbar/Toolbar.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Tools/Selection.hpp"
#include "Animation/Animation.hpp"
#include "Tools/Brush.hpp"
#include "Dialogs/Palette.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Cursor.hpp"
#include "Tools/ResizableTool.hpp"
#include "Tools/Selection.hpp"
#include "Tools/Circle.hpp"
#include "Tools/Triangle.hpp"
#include "Tools/Rectangle.hpp"
#include "Tools/Diamond.hpp"
#include "Tools/Pentagon.hpp"
#include "Tools/HexagonPointTop.hpp"
#include "Tools/HexagonFlatTop.hpp"
#include "Tools/Octagon.hpp"
#include "Dialogs/Dialog_Paste_From_File.hpp"
#include "Tools/ClipBoard.hpp"
#include "Dialogs/ConfirmDialog.hpp"
#include "Tools/Filters.hpp"


Toolbar::Toolbar() : Element() {

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(window->getSize().x, tools_height));

	// clipboard
	_btn_paste = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\btn_paste.png"), getTexture(L"tex\\tools\\btn_paste_hover.png"), getTexture(L"tex\\tools\\btn_paste_hover.png"));
	_btn_paste->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_btn_paste->_onclick_func = [this]() {

		std::shared_ptr<sf::Image> img = loadImageFromClipboard();
		if (img == nullptr)
			return;

		if (img->getSize().x > canvas->_maxSize.x || img->getSize().y > canvas->_maxSize.y) {

			std::shared_ptr<ConfirmDialog> confirm = std::make_shared<ConfirmDialog>(L"Image is too large", L"The image from the clipboard\nis too large to paste onto the canvas.\nNormalize?");

			confirm->_confirmBtn->_onclick_func = [this, confirm, img]() mutable {
				float scaleX = (float)canvas->_size.x / img->getSize().x;
				float scaleY = (float)canvas->_size.y / img->getSize().y;
				float s = std::min(scaleX, scaleY);
				sf::Vector2i size(
					(int)std::round(img->getSize().x * s),
					(int)std::round(img->getSize().y * s)
				);
				set_resize(*img, size.x, size.y);
				selection->paste(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent, *img);
				_option_transparency->_checkbox->_value = 0;
				_toolType = ToolType::Selector;
				resizable_tool = selection;
				selection->_state = ResizableToolState::Selected;
				selectToolButton(_btn_select);
				confirm->_state = DialogState::ToClose;
				};
			dialogs.push_back(confirm);
			return;
		}

		if (selection->paste(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent, *img)) {
			_option_transparency->_checkbox->_value = 0;
			_toolType = ToolType::Selector;
			resizable_tool = selection;
			selection->_state = ResizableToolState::Selected;
			selectToolButton(_btn_select);
		}
		};
	_btn_paste->setTooltip(L"Paste", L"Paste the selection from the clipboard onto the canvas");

	_option_paste = std::make_shared<Option>(L"paste", L"Ctrl+V");
	_option_paste->_onclick_func = [this]() {

		std::shared_ptr<sf::Image> img = loadImageFromClipboard();
		if (img == nullptr)
			return;

		if (img->getSize().x > canvas->_maxSize.x || img->getSize().y > canvas->_maxSize.y) {

			std::shared_ptr<ConfirmDialog> confirm = std::make_shared<ConfirmDialog>(L"Image is too large", L"The image from the clipboard\nis too large to paste onto the canvas.\nNormalize?");

			confirm->_confirmBtn->_onclick_func = [this, confirm, img]() mutable {
				float scaleX = (float)canvas->_size.x / img->getSize().x;
				float scaleY = (float)canvas->_size.y / img->getSize().y;
				float s = std::min(scaleX, scaleY);
				sf::Vector2i size(
					(int)std::round(img->getSize().x * s),
					(int)std::round(img->getSize().y * s)
				);
				set_resize(*img, size.x, size.y);
				selection->paste(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent, *img);
				_option_transparency->_checkbox->_value = 0;
				_toolType = ToolType::Selector;
				resizable_tool = selection;
				selection->_state = ResizableToolState::Selected;
				selectToolButton(_btn_select);
				confirm->_state = DialogState::ToClose;
				};
			dialogs.push_back(confirm);
			return;
		}

		if (selection->paste(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent, *img)) {
			_option_transparency->_checkbox->_value = 0;
			_toolType = ToolType::Selector;
			resizable_tool = selection;
			selection->_state = ResizableToolState::Selected;
			selectToolButton(_btn_select);
		}
		};

	_option_from_file = std::make_shared<Option>(L"from file", L"Ctrl+B");
	_option_from_file->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Paste_From_File>());
		_btn_paste_menu->_isOpen = false;
		};
	_option_transparency = std::make_shared<OptionWithCheckbox>(L"transparency", getTexture(L"tex\\unchecked.png"), getTexture(L"tex\\unchecked_hover.png"), L"Ctrl+T");
	_option_transparency->addValue(getTexture(L"tex\\checked.png"), getTexture(L"tex\\checked_hover.png"));
	_option_transparency->_onclick_func = [this]() {

		};
	_btn_paste_menu = std::make_shared<ButtonWithTopTextAndList>(L"paste", tools_text_color, tools_text_hover_color);
	_btn_paste_menu->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_btn_paste_menu->_onclick_func = [this]() {
			
		};
	_btn_paste_menu->addOption(_option_paste);
	_btn_paste_menu->addOption(_option_from_file);
	_btn_paste_menu->addOption(_option_transparency);
	

	_btn_cut = std::make_shared<ButtonWithRightText>(L"cut", tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_cut.png"), getTexture(L"tex\\tools\\btn_cut_hover.png"));
	_btn_cut->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_btn_cut->_onclick_func = [this]() {
		//selection->cut(&animation->getCurrentLayer()->image, selection->img, second_color->color);
		if (selection->_state == ResizableToolState::Selected) {
			selection->cut(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent);
		}
		};
	_btn_cut->setTooltip(L"Cut", L"Cut the selection from the canvas and place it on the clipboard");

	_btn_copy = std::make_shared<ButtonWithRightText>(L"copy", tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_copy.png"), getTexture(L"tex\\tools\\btn_copy_hover.png"));
	_btn_copy->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_btn_copy->_onclick_func = [this]() {
		if (selection->_state == ResizableToolState::Selected) {
			selection->copy(getCurrentAnimation()->getCurrentLayer()->_image, (toolbar->_option_transparency->_checkbox->_value==0)?sf::Color::Transparent : _second_color->_color);
			removeImageWithMask(getCurrentAnimation()->getCurrentLayer()->_image, selection->_resizedRect, *selection->_resizedMaskImage, sf::Color::Transparent);
		}
		};
	_btn_copy->setTooltip(L"Copy", L"Copy the selection from the canvas to the clipboard");

	_btn_select = std::make_shared<ButtonWithBottomText>(L"select", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_select.png"), getTexture(L"tex\\tools\\btn_select_hover.png"));
	_btn_select->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_btn_select->_onclick_func = [this]() {
		_toolType = ToolType::Selector;
		resizable_tool = selection;
		selectToolButton(_btn_select);
		};
	_btn_select->setTooltip(L"Selection Tool", L"Select an area of the canvas move, copy, or cut");

	_btn_lasso = std::make_shared<ButtonWithBottomText>(L"lasso", sf::Color::Transparent, tools_text_color, tools_text_hover_color, getTexture(L"tex\\tools\\btn_lasso.png"), getTexture(L"tex\\tools\\btn_lasso_hover.png"));
	_btn_lasso->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_btn_lasso->_onclick_func = [this]() {
		_toolType = ToolType::Lasso;
		resizable_tool = selection;
		selectToolButton(_btn_lasso);
		};
	_btn_lasso->setTooltip(L"Lasso Tool", L"Select an area of the canvas with a freehand selection to move, copy or cut");

	_clipboard.clear();
	_clipboard.push_back(_btn_paste);
	_clipboard.push_back(_btn_paste_menu);
	_clipboard.push_back(_btn_cut);
	_clipboard.push_back(_btn_copy);
	_clipboard.push_back(_btn_select);
	_clipboard.push_back(_btn_lasso);
	_separators.push_back(std::make_shared<Separator>());

	_clipboard_text = std::make_unique<sf::Text>(basicFont, L"clipboard", 13);
	_clipboard_text->setFillColor(tools_text_color);


	// tools
	_btn_brush = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\btn_brush.png"), getTexture(L"tex\\tools\\btn_brush_hover.png"), getTexture(L"tex\\tools\\btn_brush_press.png"));
	_btn_brush->_onclick_func = [this]() {
		_toolType = ToolType::Brush;
		resizable_tool = nullptr;
		brush->setBrushType(BrushType::Circle);
		selectToolButton(_btn_brush);
		};
	_btn_brush->setTooltip(L"Brush Tool", L"Draw on the canvas using the primary color with LMB or the secondary color with RMB");

	_btn_picker = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\btn_picker.png"), getTexture(L"tex\\tools\\btn_picker_hover.png"), getTexture(L"tex\\tools\\btn_picker_press.png"));
	_btn_picker->_onclick_func = [this]() {
		_toolType = ToolType::Picker;
		resizable_tool = nullptr;
		brush->setBrushType(BrushType::Circle);
		selectToolButton(_btn_picker);
		};
	_btn_picker->setTooltip(L"Color Picker Tool", L"Pick a color from the canvas and set it as the active color");

	_btn_fill = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\btn_fill.png"), getTexture(L"tex\\tools\\btn_fill_hover.png"), getTexture(L"tex\\tools\\btn_fill_press.png"));
	_btn_fill->_onclick_func = [this]() {
		_toolType = ToolType::Fill;
		resizable_tool = nullptr;
		selectToolButton(_btn_fill);
		};
	_btn_fill->setTooltip(L"Fill Tool", L"Fill an area of the canvas using the primary color with LMB or the secondary color with RMB");

	_btn_eraser = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\btn_eraser.png"), getTexture(L"tex\\tools\\btn_eraser_hover.png"), getTexture(L"tex\\tools\\btn_eraser_press.png"));
	_btn_eraser->_onclick_func = [this]() {
		_toolType = ToolType::Eraser;
		resizable_tool = nullptr;
		brush->setBrushType(BrushType::Square);
		selectToolButton(_btn_eraser);
		};
	_btn_eraser->setTooltip(L"Eraser Tool", L"Erase parts of the canvas using the secondary color with LMB or the primary color with RMB");

	_tools.clear();
	_tools.push_back(_btn_brush);
	_tools.push_back(_btn_picker);
	_tools.push_back(_btn_fill);
	_tools.push_back(_btn_eraser);

	for(auto& btn : _tools) {
		btn->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
			tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	}

	_tools_text = std::make_unique<sf::Text>(basicFont, L"tools", 13);
	_tools_text->setFillColor(tools_text_color);

	_toolType = ToolType::Brush;

	_separators.push_back(std::make_shared<Separator>());

	// shapes 
	_btn_circle = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\resizable_tools\\circle.png"), getTexture(L"tex\\tools\\resizable_tools\\circle_hover.png"), getTexture(L"tex\\tools\\resizable_tools\\circle_press.png"));
	_btn_circle->_onclick_func = [this]() { 
		_toolType = ToolType::Circle;
		resizable_tool = std::make_shared<Circle>();
		selectToolButton(_btn_circle);
		};

	_btn_triangle = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\resizable_tools\\triangle.png"), getTexture(L"tex\\tools\\resizable_tools\\triangle_hover.png"), getTexture(L"tex\\tools\\resizable_tools\\triangle_press.png"));
	_btn_triangle->_onclick_func = [this]() {
		_toolType = ToolType::Triangle;
		resizable_tool = std::make_shared<Triangle>();
		selectToolButton(_btn_triangle);
		};

	_btn_rectangle = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\resizable_tools\\rectangle.png"), getTexture(L"tex\\tools\\resizable_tools\\rectangle_hover.png"), getTexture(L"tex\\tools\\resizable_tools\\rectangle_press.png"));
	_btn_rectangle->_onclick_func = [this]() {
		_toolType = ToolType::Rectangle;
		resizable_tool = std::make_shared<Rectangle>();
		selectToolButton(_btn_rectangle);
		};

	_btn_diamond = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\resizable_tools\\diamond.png"), getTexture(L"tex\\tools\\resizable_tools\\diamond_hover.png"), getTexture(L"tex\\tools\\resizable_tools\\diamond_press.png"));
	_btn_diamond->_onclick_func = [this]() {
		_toolType = ToolType::Diamond;
		resizable_tool = std::make_shared<Diamond>();
		selectToolButton(_btn_diamond);
		};
	
	_btn_pentagon = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\resizable_tools\\pentagon.png"), getTexture(L"tex\\tools\\resizable_tools\\pentagon_hover.png"), getTexture(L"tex\\tools\\resizable_tools\\pentagon_press.png"));
	_btn_pentagon->_onclick_func = [this]() {
		_toolType = ToolType::Pentagon;
		resizable_tool = std::make_shared<Pentagon>();
		selectToolButton(_btn_pentagon);
		};

	_btn_hexagon_flat_top = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\resizable_tools\\hexagon_flat_top.png"), getTexture(L"tex\\tools\\resizable_tools\\hexagon_flat_top_hover.png"), getTexture(L"tex\\tools\\resizable_tools\\hexagon_flat_top_press.png"));
	_btn_hexagon_flat_top->_onclick_func = [this]() {
		_toolType = ToolType::HexagonFlatTop;
		resizable_tool = std::make_shared<HexagonFlatTop>();
		selectToolButton(_btn_hexagon_flat_top);
		};
	
	_btn_hexagon_point_top = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\resizable_tools\\hexagon_point_top.png"), getTexture(L"tex\\tools\\resizable_tools\\hexagon_point_top_hover.png"), getTexture(L"tex\\tools\\resizable_tools\\hexagon_point_top_press.png"));
	_btn_hexagon_point_top->_onclick_func = [this]() {
		_toolType = ToolType::HexagonPointTop;
		resizable_tool = std::make_shared<HexagonPointTop>();
		selectToolButton(_btn_hexagon_point_top);
		};
	
	_btn_octagon = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\resizable_tools\\octagon.png"), getTexture(L"tex\\tools\\resizable_tools\\octagon_hover.png"), getTexture(L"tex\\tools\\resizable_tools\\octagon_press.png"));
	_btn_octagon->_onclick_func = [this]() {
		_toolType = ToolType::Octagon;
		selectToolButton(_btn_octagon);
		resizable_tool = std::make_shared<Octagon>();
		};
	
	_shapes.clear();
	_shapes.push_back(_btn_circle);
	_shapes.push_back(_btn_triangle);
	_shapes.push_back(_btn_rectangle);
	_shapes.push_back(_btn_diamond);
	_shapes.push_back(_btn_pentagon);
	_shapes.push_back(_btn_hexagon_flat_top);
	_shapes.push_back(_btn_hexagon_point_top);
	_shapes.push_back(_btn_octagon);

	_btn_circle->setTooltip(L"Circle Tool", L"Draw a circle filled with the secondary color using the primary color");
	_btn_triangle->setTooltip(L"Triangle Tool", L"Draw a triangle filled with the secondary color using the primary color");
	_btn_rectangle->setTooltip(L"Rectangle Tool", L"Draw a rectangle filled with the secondary color using the primary color");
	_btn_diamond->setTooltip(L"Diamond Tool", L"Draw a diamond filled with the secondary color using the primary color");
	_btn_pentagon->setTooltip(L"Pentagon Tool", L"Draw a pentagon filled with the secondary color using the primary color");
	_btn_hexagon_flat_top->setTooltip(L"Hexagon (Flat Top) Tool", L"Draw a flat-top hexagon filled with the secondary color using the primary color");
	_btn_hexagon_point_top->setTooltip(L"Hexagon (Point Top) Tool", L"Draw a point-top hexagon filled with the secondary color using the primary color");
	_btn_octagon->setTooltip(L"Octagon Tool", L"Draw an octagon filled with the secondary color using the primary color");

	for (auto& btn : _shapes) {
		btn->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
			tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	}

	_shapes_text = std::make_unique<sf::Text>(basicFont, L"shapes", 13);
	_shapes_text->setFillColor(tools_text_color);

	_separators.push_back(std::make_shared<Separator>());

	// increase / decrease brush size
	_size_decrease = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\btn_size_decrease.png"), getTexture(L"tex\\tools\\btn_size_decrease_hover.png"), getTexture(L"tex\\tools\\btn_size_decrease_hover.png"));
	_size_decrease->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_size_decrease->_onclick_func = [this]() {
		if (_toolType == ToolType::Brush || _toolType == ToolType::Eraser)
			brush->decrease();
		};
	_size_decrease->setTooltip(L"Decrease Brush Size", L"Decrease the size of the brush or eraser");

	_size_increase = std::make_shared<ButtonWithSprite>(getTexture(L"tex\\tools\\btn_size_increase.png"), getTexture(L"tex\\tools\\btn_size_increase_hover.png"), getTexture(L"tex\\tools\\btn_size_increase_hover.png"));
	_size_increase->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_size_increase->_onclick_func = [this]() {
		if (_toolType == ToolType::Brush || _toolType == ToolType::Eraser)
			brush->increase();
		};
	_size_increase->setTooltip(L"Increase Brush Size", L"Increase the size of the brush or eraser");

	_sizes.clear();
	_sizes.push_back(_size_decrease);
	_sizes.push_back(_size_increase);

	_sizes_text = std::make_unique<sf::Text>(basicFont, L"size", 13);
	_sizes_text->setFillColor(tools_text_color);

	_separators.push_back(std::make_shared<Separator>());

	// main colors
	_first_color = std::make_shared<LargeColorButton>(sf::Color::Black);
	_first_color->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_second_color = std::make_shared<LargeColorButton>(sf::Color::White);
	_second_color->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color, 
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);

	_selectedColorButton = _first_color;
	_selectedColorButton->setSelect(true);

	_first_color->_onclick_func = [this]() {
		selectColorButton(_first_color);
		};

	_second_color->_onclick_func = [this]() {
		selectColorButton(_second_color);
		};

	_first_color->setTooltip(L"Primary Color", L"The primary color is used for drawing with the left mouse button");
	_second_color->setTooltip(L"Secondary Color", L"The secondary color is used for drawing with the right mouse button");

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
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(0, 0, 0)));
	_colors.back()->setTooltip(L"Black", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(255, 255, 255)));
	_colors.back()->setTooltip(L"White", L"");

	// greys
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(63, 63, 63)));
	_colors.back()->setTooltip(L"Dark Grey", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(159, 159, 159)));
	_colors.back()->setTooltip(L"Light Grey", L"");

	// red
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(63, 0, 0)));
	_colors.back()->setTooltip(L"Dark Red", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(191, 0, 0)));
	_colors.back()->setTooltip(L"Light Red", L"");

	// orange
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(127, 63, 0)));
	_colors.back()->setTooltip(L"Dark Orange", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(255, 127, 0)));
	_colors.back()->setTooltip(L"Light Orange", L"");

	// yellow
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(63, 63, 0)));
	_colors.back()->setTooltip(L"Dark Yellow", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(191, 191, 0)));
	_colors.back()->setTooltip(L"Light Yellow", L"");

	// green
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(0, 63, 0)));
	_colors.back()->setTooltip(L"Dark Green", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(0, 191, 0)));
	_colors.back()->setTooltip(L"Light Green", L"");

	// cyan
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(0, 63, 63)));
	_colors.back()->setTooltip(L"Dark Cyan", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(0, 191, 191)));
	_colors.back()->setTooltip(L"Light Cyan", L"");

	// blue
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(0, 0, 63)));
	_colors.back()->setTooltip(L"Dark Blue", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(0, 0, 191)));
	_colors.back()->setTooltip(L"Light Blue", L"");

	// magenta
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(63, 0, 63)));
	_colors.back()->setTooltip(L"Dark Magenta", L"");
	_colors.push_back(std::make_shared<ColorButton>(sf::Color(191, 0, 191)));
	_colors.back()->setTooltip(L"Light Magenta", L"");

	for (auto& c : _colors) {
		c->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
			tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
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
	_btn_palette_colors->setRectColors(tools_button_idle_color, tools_button_hover_color, tools_button_press_color, tools_button_select_color, tools_button_inactive_color,
		tools_border_width, tools_button_idle_border_color, tools_button_hover_border_color, tools_button_press_border_color, tools_button_select_border_color, tools_button_inactive_border_color);
	_btn_palette_colors->_onclick_func = [this]() {
		if(palette == nullptr)
			palette = std::make_shared<Palette>();
			static_dialogs.push_back(palette);
		};
	_btn_palette_colors->setTooltip(L"Color Palette", L"Open the color palette dialog to create and manage custom colors");

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

	position = sf::Vector2i(_rect.position) + sf::Vector2i(optionbox_border_width, 0);
	
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

	_separators[1]->setPosition(sf::Vector2i(x, menu_height));
	x += _separators[1]->getSize().x;

	// shapes
	old_x = x;
	y = menu_height + 4;

	x += 2;
	for (int i = 0; i < _shapes.size()/2; i += 1) {
		_shapes[i]->setPosition(sf::Vector2i(x, y));
		_shapes[_shapes.size()/2 + i]->setPosition(sf::Vector2i(x, y + 32));
		x += 32;
	}

	x += 2;

	_shapes_text->setPosition(sf::Vector2f((x + old_x) / 2 - _shapes_text->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	_separators[2]->setPosition(sf::Vector2i(x, menu_height));
	x += _separators[2]->getSize().x;

	// sizes
	old_x = x;
	x += 2;
	_sizes[0]->setPosition(sf::Vector2i(x, menu_height + 4));
	_sizes[1]->setPosition(sf::Vector2i(x, menu_height + 32 + 4));
	x += 32;
	x += 2;

	_sizes_text->setPosition(sf::Vector2f((x + old_x) / 2 - _sizes_text->getGlobalBounds().size.x / 2.0f, menu_height + tools_height - basicFont.getLineSpacing(14) - 4));

	_separators[3]->setPosition(sf::Vector2i(x, menu_height));
	x += _separators[3]->getSize().x;

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

	_separators[4]->setPosition(sf::Vector2i(x, menu_height));
	x += _separators[4]->getSize().x;

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
		_selectedToolButton->setSelect(false);
	}

	_selectedToolButton = toolButton;

	if (_selectedToolButton != nullptr) {
		_selectedToolButton->setSelect(true);
	}
}

void Toolbar::selectColorButton(std::shared_ptr<LargeColorButton> colorButton) {
	if (_selectedColorButton != nullptr) {
		_selectedColorButton->setSelect(false);
	}

	_selectedColorButton = colorButton;

	if (_selectedColorButton != nullptr) {
		_selectedColorButton->setSelect(true);
	}
}

void Toolbar::cursorHover() {

	if (!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen) {
		toolbar->_btn_paste_menu->cursorHover();
		return;
	}

	if (canvas->_state != CanvasState::Idle)
		return;

	if (!(resizable_tool!= nullptr && (resizable_tool->_state == ResizableToolState::None || resizable_tool->_state == ResizableToolState::Selected)))
		return;

	if (!static_dialogs.empty() && static_dialogs.front()->_is_moved)
		return;
	
	if (_rect.contains(cursor->_position)) {
		Element_hovered = this->shared_from_this();
	}

	for (auto& tool : _clipboard)
		tool->cursorHover();

	for (auto& tool : _tools)
		tool->cursorHover();

	for (auto& shape : _shapes)
		shape->cursorHover();

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

	if (toolbar->_btn_paste_menu->_isOpen) {
		toolbar->_btn_paste_menu->handleEvent(event);
		return;
	}

	if (_rect.contains(cursor->_position)) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			Element_pressed = this->shared_from_this();
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
			_toolType = ToolType::Selector;
			selectToolButton(_btn_select);
			selection->selectAll();
		}else if(sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && kp->code == sf::Keyboard::Key::D) {
			_toolType = ToolType::Selector;
			selectToolButton(_btn_select);
			selection->unselect();

		}

	}

	for (auto& tool : _clipboard)
		tool->handleEvent(event);

	for (auto& tool : _tools)
		tool->handleEvent(event);

	for (auto& shape : _shapes)
		shape->handleEvent(event);

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

	for (auto& shape : _shapes)
		shape->update();

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
	window->draw(*_shapes_text);
	window->draw(*_sizes_text);
	window->draw(*_first_color_text_col);
	window->draw(*_first_color_text_val);
	window->draw(*_second_color_text_col);
	window->draw(*_second_color_text_val);
	window->draw(*_colors_text);
	
	for (auto& tool : _tools) tool->draw();
	for (auto& shape : _shapes) shape->draw();
	for (auto size : _sizes) size->draw();
	_first_color->draw();
	_second_color->draw();
	for (auto& col : _colors) col->draw();
	_btn_palette_colors->draw();
	for (auto& separator : _separators) separator->draw();
	for (auto& tool : _clipboard) tool->draw();

	
}

std::shared_ptr<Toolbar> toolbar;