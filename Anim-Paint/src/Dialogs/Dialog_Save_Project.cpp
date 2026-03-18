#include "Dialogs/Dialog_Save_Project.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Dialogs/ConfirmDialog.hpp"

Dialog_Save_Project::Dialog_Save_Project() : FileDialog(L"Save Project", L"save", L".animproj") {

	_selectBtn->_onclick_func = [this]() {
		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;

		if (!(fullPath.extension() == ".animproj"))
			fullPath += L".animproj";

		if (std::filesystem::exists(fullPath)) {
			std::shared_ptr<ConfirmDialog> confirm = std::make_shared<ConfirmDialog>(L"File Already Exists", L"Do you want to replace it?");
			confirm->_confirmBtn->_onclick_func = [this, fullPath, confirm]() {
				main_menu->saveProject(fullPath);
				_state = DialogState::ToClose;
				confirm->_state = DialogState::ToClose;
				};
			confirm->_cancelBtn->_onclick_func = [this, confirm]() {
				confirm->_state = DialogState::ToClose;
				};
			dialogs.push_back(confirm);
		}
		else {
			main_menu->saveProject(fullPath);
			_state = DialogState::ToClose;
		}
		};

	_onTabElements.push_back(_filenameInput);
	_onTabElements.push_back(_selectBtn);
	_onTabElements.push_back(_cancelBtn);
}

Dialog_Save_Project::~Dialog_Save_Project() {

}
