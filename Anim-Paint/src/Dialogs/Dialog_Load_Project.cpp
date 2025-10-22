#include "Dialogs/Dialog_Load_Project.hpp"
#include "MainMenu.hpp"
#include <iostream>

Dialog_Load_Project::Dialog_Load_Project() : FileDialog(L"Load Project", L"load") {

	this->_selectBtn->_onclick_func = [this]() {
		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;
		//std::wcout << L"Load: " << fullPath << L"\n";
		main_menu->loadProject(fullPath);
		_state = DialogState::ToClose;
		};
}

Dialog_Load_Project::~Dialog_Load_Project() {

}
