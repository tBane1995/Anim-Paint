#include "Dialogs/Dialog_Load_Project.hpp"
#include "Components/MainMenu.hpp"
#include <iostream>
#include "Animation/Animation.hpp"
#include "Components/Canvas.hpp"
#include "../include/Components/AnimationsDialog.hpp"
#include "../include/Components/FramesDialog.hpp"
#include "../include/Components/LayersDialog.hpp"
#include "Components/Canvas.hpp"
#include "History.hpp"

Dialog_Load_Project::Dialog_Load_Project() : FileDialog(L"Load Project", L"load") {

	this->_selectBtn->_onclick_func = [this]() {
		std::wstring name = _filenameInput->getText();
		std::filesystem::path fullPath = std::filesystem::path(currentPath) / name;
		//std::wcout << L"Load: " << fullPath << L"\n";
		
		main_menu->loadProject(fullPath);
		
		_state = DialogState::ToClose;

		firstAnimation();
		getCurrentAnimation()->firstFrame();
		getCurrentAnimation()->firstLayer();

		history->clear();
		history->saveStep();

		sf::Vector2i newCanvasSize = sf::Vector2i(getCurrentAnimation()->getCurrentLayer()->_image.getSize());
		canvas->resize(newCanvasSize);

		animations_dialog->updateText();
		frames_dialog->updateText();
		layers_dialog->loadLayersFromCurrentFrame();
		};

	_onTabElements.push_back(_filenameInput);
	_onTabElements.push_back(_selectBtn);
	_onTabElements.push_back(_cancelBtn);
}

Dialog_Load_Project::~Dialog_Load_Project() {

}
