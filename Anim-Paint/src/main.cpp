#include <iostream>
#include <string>
#include <functional>
#define NOMINMAX
#include <windows.h>
#include <fcntl.h>
#include <io.h>

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
#include "Dialogs/Dialog_Save_As.hpp"
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
	Dialog* preview = new Dialog(L"Preview", sf::Vector2f(192, 192), sf::Vector2f(window->getSize().x - 192 - dialog_margin, main_menu->getSize().y + tools_height + dialog_margin));
	dialogs.push_back(preview);
	
	frames_dialog = new FramesDialog(L"Frames",
		sf::Vector2f(192, 32 + 32 + dialog_padding * 2),
		dialogs.back()->getPosition() + sf::Vector2f(0, dialogs.back()->getSize().y + dialog_margin));
	dialogs.push_back(frames_dialog);

	layers_dialog = new LayersDialog(L"Layers", 
		sf::Vector2f(160, 32 + 4 * 32 + dialog_padding * 2), 
		sf::Vector2f(window->getSize().x - 160 - dialog_margin, dialogs.back()->getPosition().y + dialogs.back()->getSize().y + dialog_margin));
	dialogs.push_back(layers_dialog);

}

int main() {
	_setmode(_fileno(stdout), _O_U16TEXT); // wide char UTF-16 output

	window  = new sf::RenderWindow(sf::VideoMode(800,600), "Anim Paint", sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);

	loadTextures();
	loadTheme();
	main_menu = new MainMenu();
	animation = new Animation();
	createDialogs();
	selection = new Selection();
	lasso = new Lasso();
	brush = new Brush(2);
	toolbar = new Toolbar();
	canvas = new Canvas(sf::Vector2i(animation->getLayer(0)->_image.getSize()));

	dialogs.push_back(new Dialog_Save_As());

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

		// cursor hovering
		ElementGUI_hovered = nullptr;

		toolbar->cursorHover();
		main_menu->cursorHover();
		canvas->cursorHover();
		

		for (auto& dialog : dialogs)
			dialog->cursorHover();

		// Element GUI update before handle events
		
		for (auto& dialog : dialogs)
			dialog->update();
		toolbar->update();
		
		main_menu->update();
		canvas->update();


		// handle events
		sf::Event event;
		while (window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window->close();

			if (event.type == sf::Event::MouseButtonPressed) {

				toolbar->handleEvent(event);
				main_menu->handleEvent(event);
				
				canvas->handleEvent(event);

				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
				
				
				
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				
				toolbar->handleEvent(event);
				main_menu->handleEvent(event);
				
				
				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
				canvas->handleEvent(event);
			}
			else if (event.type == sf::Event::MouseMoved) {
				toolbar->handleEvent(event);
				main_menu->handleEvent(event);
				
				
				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
				canvas->handleEvent(event);
			}
			else if (event.type == sf::Event::MouseWheelScrolled) {
				canvas->handleEvent(event);
			}
			else if (event.type == sf::Event::KeyPressed) {
				
				if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::X) {
					toolbar->_btn_cut->click();
				}
				else if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::C) {
					toolbar->_btn_copy->click();
				}
				else if (event.type == sf::Event::KeyPressed && sf::Keyboard::isKeyPressed(sf::Keyboard::LControl) && event.key.code == sf::Keyboard::V) {
					toolbar->_btn_paste->click();
				}
				else {
					for (auto& dialog : dialogs) {
						dialog->handleEvent(event);
					}
				}
				
			}

				

		}
		
		// update
		
		// render
		window->clear(sf::Color(56,56,56));
		canvas->draw();
		
		
		toolbar->draw();
		main_menu->draw();

		for (auto& dialog : dialogs)
			dialog->draw();
		window->display();


	}

	
}


