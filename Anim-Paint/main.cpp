#include <iostream>
#include <string>
#include <functional>
#include <SFML/Graphics.hpp>

#include "Window.hpp"
#include "Textures.hpp"
#include "Theme.hpp"
#include "Mouse.hpp"
#include "Time.hpp"
#include "ElementGUI.hpp"
#include "Button.hpp"
#include "Checkbox.hpp"
#include "Dialog.hpp"

#include "Layer.hpp"
#include "Frame.hpp"

#include "MainMenu.hpp"
#include "ColorsDialog.hpp"
#include "FramesDialog.hpp"
#include "LayersDialog.hpp"
#include "Canvas.hpp"

void createDialogs() {
	Dialog* preview = new Dialog(L"Preview", sf::Vector2f(192, 192), sf::Vector2f(window->getSize().x - 192 - dialog_margin, main_menu->getSize().y + dialog_margin));
	dialogs.push_back(preview);
	
	frames_dialog = new FramesDialog(L"Frames",
		sf::Vector2f(192, 32 + 32 + dialog_padding * 2),
		dialogs.back()->getPosition() + sf::Vector2f(0, dialogs.back()->getSize().y + dialog_margin));
	dialogs.push_back(frames_dialog);

	layers_dialog = new LayersDialog(L"Layers", 
		sf::Vector2f(160, 32 + 4 * 32 + dialog_padding * 2), 
		sf::Vector2f(window->getSize().x - 160 - dialog_margin, dialogs.back()->getPosition().y + dialogs.back()->getSize().y + dialog_margin));
	dialogs.push_back(layers_dialog);

	dialogs.push_back(new Dialog(L"Tools", sf::Vector2f(window->getSize().x - preview->getSize().x - 3* dialog_margin, 128), sf::Vector2f(dialog_margin, main_menu->getSize().y + dialog_margin)));

	colors_dialog = new ColorsDialog(L"Colors",
		sf::Vector2f(152, 286),
		sf::Vector2f(dialog_margin, dialogs.back()->getPosition().y + dialogs.back()->getSize().y + dialog_margin));
	dialogs.push_back(colors_dialog);
}

int main() {
	
	loadTextures();
	loadTheme();
	main_menu = new MainMenu();
	createDialogs();

	canvas = new Canvas(sf::Vector2i(frames_dialog->getCurrentFrame()->layers[0]->image.getSize()));

	while (window->isOpen()) {
		prevTime = currentTime;
		currentTime = timeClock.getElapsedTime();

		mousePosition = sf::Mouse::getPosition(*window); // get the mouse position about window
		worldMousePosition = window->mapPixelToCoords(mousePosition); // get global mouse position
		
		// cursor hovering
		ElementGUI_hovered = nullptr;

		canvas->cursorHover();
		main_menu->cursorHover();

		for (auto& dialog : dialogs)
			dialog->cursorHover();

		// Element GUI update before handle events
		canvas->update();
		for (auto& dialog : dialogs)
			dialog->update();
		main_menu->update();


		// handle events
		sf::Event event;
		while (window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window->close();

			if (event.type == sf::Event::MouseButtonPressed) {

				canvas->handleEvent(event);
				main_menu->handleEvent(event);
				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
				
			}
			else if (event.type == sf::Event::MouseButtonReleased) {

				canvas->handleEvent(event);
				main_menu->handleEvent(event);
				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
			}
			else if (event.type == sf::Event::MouseMoved) {
				canvas->handleEvent(event);
				main_menu->handleEvent(event);
				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
			}
			else if (event.type == sf::Event::MouseWheelScrolled) {
				canvas->handleEvent(event);
			}
			else if (event.type == sf::Event::KeyPressed) {
				for (auto& dialog : dialogs) {
					dialog->handleEvent(event);
				}
			}

				

		}
		
		// update
		

		// render
		window->clear(sf::Color(56,56,56));
		canvas->draw();
		main_menu->draw();
		for (auto& dialog : dialogs) 
			dialog->draw();
		
		window->display();


	}
	

	return 0;
}