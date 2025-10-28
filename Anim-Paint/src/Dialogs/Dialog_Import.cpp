#include "Dialogs/Dialog_Import.hpp"
#include "Dialogs/Dialog_Load_SpriteSheet.hpp"
#include <iostream>

Dialog_Import::Dialog_Import() : FileDialog(L"Import From File", L"import") {

	

	this->_selectBtn->_onclick_func = [this]() {

		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;
		if (!(fullPath.extension() == ".png"))
			fullPath += L".png";
		std::wcout << L"Import: " << fullPath << L"\n";
		dialogs.push_back(new Dialog_Load_SpriteSheet(fullPath));
		_state = DialogState::ToClose;
	};

}

Dialog_Import::~Dialog_Import() {

}
