#include "Dialogs/Dialog_Paste_From_File.hpp"
#include "Dialogs/Dialog_Load_SpriteSheet.hpp"
#include <iostream>
#include "Tools/ClipBoard.hpp"
#include "Tools/Selection.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"
#include "Dialogs/ConfirmDialog.hpp"
#include "Tools/Filters.hpp"

Dialog_Paste_From_File::Dialog_Paste_From_File() : FileDialog(L"Paste From File", L"paste", L".png") {

	this->_selectBtn->_onclick_func = [this]() {

		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;
		if (!(fullPath.extension() == ".png"))
			fullPath += L".png";

		std::shared_ptr<sf::Image> img = std::make_shared<sf::Image>(fullPath);

		if (img == nullptr)
			return;

		if (img->getSize().x > canvas->_size.x || img->getSize().y > canvas->_size.y) {

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
				toolbar->_option_transparency->_checkbox->_value = 0;
				toolbar->_toolType = ToolType::Selector;
				selection->_state = SelectionState::Selected;
				toolbar->selectToolButton(toolbar->_btn_select);
				confirm->_state = DialogState::ToClose;
				};
			dialogs.push_back(confirm);
		}
		else {
			selection->paste(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent, *img);
			toolbar->_option_transparency->_checkbox->_value = 0;
			toolbar->_toolType = ToolType::Selector;
			selection->_state = SelectionState::Selected;
			toolbar->selectToolButton(toolbar->_btn_select);
		}

		_state = DialogState::ToClose;
	};

	_onTabElements.push_back(_filenameInput);
	_onTabElements.push_back(_selectBtn);
	_onTabElements.push_back(_cancelBtn);

}

Dialog_Paste_From_File::~Dialog_Paste_From_File() {

}
