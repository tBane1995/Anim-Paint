#include "Dialogs/Dialog_Export.hpp"
#include "MainMenu.hpp"
#include <iostream>

Dialog_Export::Dialog_Export() : FileDialog(L"Export As File", L"export") {

	this->_selectBtn->_onclick_func = [this]() {
		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;
		if (!(fullPath.extension() == ".png"))
			fullPath += L".png";
		//std::wcout << L"Export: " << fullPath << L"\n";
		main_menu->exportAsFile(fullPath);
		_state = DialogState::ToClose;
		};
}

Dialog_Export::~Dialog_Export() {

}
