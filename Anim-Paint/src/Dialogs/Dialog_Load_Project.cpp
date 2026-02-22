#include "Dialogs/Dialog_Load_Project.hpp"
#include "Components/MainMenu.hpp"
#include <iostream>
#include "Animation/Animation.hpp"
#include "Components/Canvas.hpp"
#include "../include/Components/AnimationsPanel.hpp"
#include "../include/Components/FramesPanel.hpp"
#include "../include/Components/LayersPanel.hpp"
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

		animations_panel->updateText();
		frames_panel->updateText();
		layers_panel->loadLayersFromCurrentFrame();
		};

	_onTabElements.push_back(_filenameInput);
	_onTabElements.push_back(_selectBtn);
	_onTabElements.push_back(_cancelBtn);
}

Dialog_Load_Project::~Dialog_Load_Project() {

}
