#include <iostream>
#include <string>
#include <functional>
#define NOMINMAX
#include <windows.h>
#include <fcntl.h>
#include <io.h>

#include "SFML/Window/VideoMode.hpp"
#include "SFML/Graphics.hpp"

#include "Window.hpp"
#include "Textures.hpp"
#include "Theme.hpp"
#include "Mouse.hpp"
#include "Time.hpp"

#include "ElementGUI/ElementGUI.hpp"
#include "ElementGUI/Button.hpp"
#include "ElementGUI/Checkbox.hpp"
#include "ElementGUI/Slider.hpp"



#include "WorldToTileConverter.hpp"
#include "ClipBoard.hpp"
#include "Filters.hpp"

#include "Animation/Layer.hpp"
#include "Animation/Frame.hpp"
#include "Animation/Animation.hpp"

#include "Dialogs/Dialog.hpp"
#include "Dialogs/FramesDialog.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Dialogs/FileDialog.hpp"
#include "Dialogs/Dialog_Rotation.hpp"
#include "Dialogs/Dialog_Brightness_Contrast.hpp"
#include "Dialogs/Dialog_Saturation.hpp"
#include "Dialogs/Dialog_Sepia.hpp"

#include "MainMenu.hpp"

#include "Tools/Selection.hpp"
#include "Tools/Lasso.hpp"
#include "Tools/Brush.hpp"
#include "Tools/Toolbar.hpp"
#include "Canvas.hpp"

void createDialogs() {
	
	frames_dialog = new FramesDialog(L"Frames",
		sf::Vector2f(192, 32 + 32 + dialog_padding * 2),
		sf::Vector2f(window->getSize().x - 192 - dialog_margin, main_menu->getSize().y + toolbar->_rect.getSize().y + dialog_margin));

	layers_dialog = new LayersDialog(L"Layers", 
		sf::Vector2f(160, 32 + 4 * 32 + dialog_padding * 2), 
		sf::Vector2f(window->getSize().x - 160 - dialog_margin, frames_dialog->getPosition().y + frames_dialog->getSize().y + dialog_margin));
	
}

int main() {
	_setmode(_fileno(stdout), _O_U16TEXT); // wide char UTF-16 output
	window = new sf::RenderWindow(sf::VideoMode({800, 600}), "Anim Paint", sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
	window->setFramerateLimit(60);
	window->setView(mainView);
	//window->setKeyRepeatEnabled(false);
	loadTextures();
	loadTheme();
	main_menu = new MainMenu();
	animation = new Animation();
	animation->addEmptyFrame(sf::Vector2i(64,64));
	selection = new Selection();
	lasso = new Lasso();
	brush = new Brush(2);
	toolbar = new Toolbar();
	createDialogs();
	canvas = new Canvas(sf::Vector2i(animation->getLayer(0)->_image.getSize()));

	// TO-DO - for the tests
	main_menu->_menu_boxes[0]->_options[5]->click();


	// FPS counter
	sf::Clock fpsClock;
	float fps = 0.0f;
	int frameCount = 0;


	while (window->isOpen()) {

		prevTime = currentTime;
		currentTime = mainClock.getElapsedTime();

		mousePosition = sf::Mouse::getPosition(*window); // get the mouse position about window
		worldMousePosition = window->mapPixelToCoords(mousePosition); // get global mouse position
		 
		// delete old dialogs
		for (int i = 0; i < dialogs.size(); ) {
			if (dialogs[i]->_state == DialogState::ToClose) {
				delete dialogs[i];
				dialogs.erase(dialogs.begin() + i);
			}
			else {
				i++;
			}
		}

		// calculate the FPS
		frameCount++;

		if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
			fps = frameCount / fpsClock.restart().asSeconds();
			frameCount = 0;
			window->setTitle(L"Anim Paint - FPS: " + std::to_wstring(int(fps)));
		}

		// cursor hovering
		ElementGUI_hovered = nullptr;

		toolbar->cursorHover();
		main_menu->cursorHover();
		canvas->cursorHover();
		frames_dialog->cursorHover();
		layers_dialog->cursorHover();


		for (auto& dialog : dialogs)
			dialog->cursorHover();

		// Element GUI update before handle events
		for (auto& dialog : dialogs)
			dialog->update();
		toolbar->update();

		main_menu->update();
		canvas->update();
		frames_dialog->update();
		layers_dialog->update();

		// handle events
		while (const std::optional event = window->pollEvent()) {

			if (event->is<sf::Event::Closed>())
				window->close();

			toolbar->handleEvent(*event);
			main_menu->handleEvent(*event);

			for (auto& dialog : dialogs)
				dialog->handleEvent(*event);

			frames_dialog->handleEvent(*event);
			layers_dialog->handleEvent(*event);

			canvas->handleEvent(*event);

			if (const auto* kp = event->getIf<sf::Event::KeyPressed>()) {

				if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && kp->code == sf::Keyboard::Key::X) {
					toolbar->_btn_cut->click();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && kp->code == sf::Keyboard::Key::C) {
					toolbar->_btn_copy->click();
				}
				else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl) && kp->code == sf::Keyboard::Key::V) {
					toolbar->_btn_paste->click();
				}

			}
		}
		
		// update
		
		// render
		window->clear(sf::Color(56, 56, 56));
		canvas->draw();
		toolbar->draw();
		main_menu->draw();

		frames_dialog->draw();
		layers_dialog->draw();

		for (auto& dialog : dialogs)
			dialog->draw();
		window->display();

		
	}

	
}


