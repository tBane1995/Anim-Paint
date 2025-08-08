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
#include "Dialog.hpp"
#include "Canvas.hpp"
#include "Frames.hpp"
#include "Layers.hpp"

void createDialogs() {
	dialogs.push_back(new Dialog(L"Preview", sf::Vector2f(192, 256), sf::Vector2f(window->getSize().x - 192 - dialog_margin, dialog_margin)));
	dialogs.push_back(new Frames(L"Frames", sf::Vector2f(192, 32 + 32 + dialog_padding * 2), dialogs.back()->getPosition() + sf::Vector2f(0,dialogs.back()->getSize().y + dialog_margin)));
	dialogs.push_back(new Layers(L"Layers", sf::Vector2f(160, 32 + 4 * 32 + dialog_padding*2), sf::Vector2f(window->getSize().x - 160 - dialog_margin, dialogs.back()->getPosition().y + dialogs.back()->getSize().y + dialog_margin)));
	dialogs.push_back(new Dialog(L"Tools", sf::Vector2f(128, 256), sf::Vector2f(dialog_margin, dialog_margin)));
	dialogs.push_back(new Dialog(L"Colors", sf::Vector2f(128, 256), sf::Vector2f(dialog_margin, window->getSize().y - 256 - dialog_margin)));

}

int main() {
	
	loadTextures();
	loadTheme();
	canvas = new Canvas(sf::Vector2f(16, 16));

	createDialogs();

	while (window->isOpen()) {
		prevTime = currentTime;
		currentTime = timeClock.getElapsedTime();

		mousePosition = sf::Mouse::getPosition(*window); // get the mouse position about window
		worldMousePosition = window->mapPixelToCoords(mousePosition); // get global mouse position
		
		// cursor hovering
		ElementGUI_hovered = nullptr;

		for (auto& dialog : dialogs)
			dialog->cursorHover();

		// Element GUI update before handle events
		for (auto& dialog : dialogs)
			dialog->update();
		// handle events
		
		sf::Event event;
		while (window->pollEvent(event)) {

			if (event.type == sf::Event::Closed)
				window->close();

			if (event.type == sf::Event::MouseButtonPressed) {
				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
			}
			else if (event.type == sf::Event::MouseButtonReleased) {
				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
			}
			else if(event.type == sf::Event::MouseMoved)
				for (auto& dialog : dialogs)
					dialog->handleEvent(event);
		}
		
		// update
		

		// render
		window->clear(sf::Color(48,48,48));
		canvas->draw();
		for (auto& dialog : dialogs) 
			dialog->draw();
		
		window->display();


	}
	

	return 0;
}