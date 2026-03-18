#include "Dialogs/Dialog_Export.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include <iostream>
#include "Dialogs/ConfirmDialog.hpp"

Dialog_Export::Dialog_Export() : FileDialog(L"Export As File", L"export", L".png") {

	_selectBtn->_onclick_func = [this]() {
		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;

		if (!(fullPath.extension() == ".png"))
			fullPath += L".png";

		if (std::filesystem::exists(fullPath)) {
			std::shared_ptr<ConfirmDialog> confirm = std::make_shared<ConfirmDialog>(L"File Already Exists", L"Do you want to replace it?");
			confirm->_confirmBtn->_onclick_func = [this, fullPath, confirm]() {
				main_menu->exportAsFile(fullPath);
				_state = DialogState::ToClose;
				confirm->_state = DialogState::ToClose;
				};
			confirm->_cancelBtn->_onclick_func = [this, confirm]() {
				confirm->_state = DialogState::ToClose;
				};
			dialogs.push_back(confirm);
		}
	};


	_onTabElements.push_back(_filenameInput);
	_onTabElements.push_back(_selectBtn);
	_onTabElements.push_back(_cancelBtn);
}

Dialog_Export::~Dialog_Export() {

}
