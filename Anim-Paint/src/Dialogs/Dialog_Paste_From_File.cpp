#include "Dialogs/Dialog_Paste_From_File.hpp"
#include "Dialogs/Dialog_Load_SpriteSheet.hpp"
#include <iostream>
#include "Tools/ClipBoard.hpp"
#include "Tools/Selection.hpp"
#include "Components/Toolbar/Toolbar.hpp"
Dialog_Paste_From_File::Dialog_Paste_From_File() : FileDialog(L"Paste From File", L"paste", L".png") {

	this->_selectBtn->_onclick_func = [this]() {

		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;
		if (!(fullPath.extension() == ".png"))
			fullPath += L".png";

		sf::Image pasteImage(fullPath);
		copyImageToClipboard(pasteImage, sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(pasteImage.getSize())));

		if (selection->paste(getCurrentAnimation()->getCurrentLayer()->_image, sf::Color::Transparent)) {
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
