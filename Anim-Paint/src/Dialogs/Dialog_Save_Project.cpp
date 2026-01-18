#include "Dialogs/Dialog_Save_Project.hpp"
#include "MainMenu.hpp"
#include <iostream>

Dialog_Save_Project::Dialog_Save_Project() : FileDialog(L"Save Project", L"save") {

	this->_selectBtn->_onclick_func = [this]() {
		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;
		//std::wcout << L"Save: " << fullPath << L"\n";
		main_menu->saveProject(fullPath);
		_state = DialogState::ToClose;
		};

	_onTabElements.push_back(_filenameInput);
	_onTabElements.push_back(_selectBtn);
	_onTabElements.push_back(_cancelBtn);
}

Dialog_Save_Project::~Dialog_Save_Project() {

}
