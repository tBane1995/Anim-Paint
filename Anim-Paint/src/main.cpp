#include <iostream>
#include <string>
#include <functional>
#define NOMINMAX
#include <windows.h>
#include <fcntl.h>
#include <io.h>

#include "SFML/Window.hpp"
#include "SFML/Graphics.hpp"

#include "Window.hpp"
#include "Textures.hpp"
#include "Theme.hpp"
#include "Cursor.hpp"
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
#include "Dialogs/AnimationsDialog.hpp"
#include "Dialogs/Palette.hpp"
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
	
	animations_dialog = std::make_shared<AnimationsDialog>(L"Animations",
		sf::Vector2i(192, dialog_title_rect_height + 32 + 32 + dialog_padding * 2),
		sf::Vector2i(window->getSize().x - 192 - dialog_margin, main_menu->getSize().y + toolbar->_rect.size.y + dialog_margin));

	frames_dialog = std::make_shared<FramesDialog>(L"Frames",
		sf::Vector2i(192, dialog_title_rect_height + 32 + 32 + dialog_padding * 2),
		sf::Vector2i(window->getSize().x - 192 - dialog_margin, animations_dialog->getPosition().y + animations_dialog->getSize().y + dialog_margin));

	layers_dialog = std::make_shared<LayersDialog>(L"Layers", 
		sf::Vector2i(160, dialog_title_rect_height + 4 * 32 + 32 + dialog_padding * 2),
		sf::Vector2i(window->getSize().x - 160 - dialog_margin, frames_dialog->getPosition().y + frames_dialog->getSize().y + dialog_margin));
	
}

int main() {
	_setmode(_fileno(stdout), _O_U16TEXT); // wide char UTF-16 output
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(mainView.getSize().x, mainView.getSize().y)), "Anim Paint", sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
	
	//window->setKeyRepeatEnabled(false);
	loadTextures();
	loadTheme();

	window->setFramerateLimit(120);
	window->setView(mainView);

	cursor = new Cursor();
	main_menu = std::make_shared<MainMenu>();

	std::shared_ptr<Animation> animation = std::make_shared<Animation>();
	animation->addEmptyFrame(sf::Vector2i(64,64));
	animations.push_back(animation);

	selection = new Selection();
	lasso = std::make_shared<Lasso>();
	brush = std::make_shared<Brush>(2);
	toolbar = std::make_shared<Toolbar>();
	createDialogs();
	canvas = std::make_shared<Canvas>();

	// FPS counter
	sf::Clock fpsClock;
	float fps = 0.0f;
	int frameCount = 0;


	while (window->isOpen()) {

		prevTime = currentTime;
		currentTime = mainClock.getElapsedTime();

		cursor->update();

		// delete old dialogs
		for (int i = 0; i < dialogs.size(); ) {
			if (dialogs[i]->_state == DialogState::ToClose) {
				dialogs.erase(dialogs.begin() + i);
			}
			else {
				i++;
			}
		}

		if (palette && palette->_state == DialogState::ToClose)
			palette = nullptr;

		// calculate the FPS
		frameCount++;

		if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
			fps = frameCount / fpsClock.restart().asSeconds();
			frameCount = 0;
			window->setTitle(L"Anim Paint - FPS: " + std::to_wstring(int(fps)));
		}

		// cursor hovering
		ElementGUI_hovered = nullptr;

		canvas->cursorHover();
		toolbar->cursorHover();
		
		frames_dialog->cursorHover();
		layers_dialog->cursorHover();
		animations_dialog->cursorHover();

		if (palette)
			palette->cursorHover();

		for (auto& dialog : dialogs)
			dialog->cursorHover();
		main_menu->cursorHover();


		// Element GUI update before handle events
		for (auto& dialog : dialogs)
			dialog->update();
		toolbar->update();

		
		canvas->update();
		frames_dialog->update();
		layers_dialog->update();
		animations_dialog->update();
		if (palette)
			palette->update();
		main_menu->update();

		// handle events
		while (const std::optional event = window->pollEvent()) {

			if (event->is<sf::Event::Closed>())
				window->close();

			for (auto& dialog : dialogs) {
				dialog->handleEvent(*event);
				continue;
			}

			main_menu->handleEvent(*event);

			canvas->handleEvent(*event);
			toolbar->handleEvent(*event);

			frames_dialog->handleEvent(*event);
			layers_dialog->handleEvent(*event);
			animations_dialog->handleEvent(*event);

			if (palette)
				palette->handleEvent(*event);

			

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

			cursor->handleEvent(*event);
		}
		
		// update
		
		// render
		window->clear(sf::Color(56, 56, 56));
		canvas->draw();
		toolbar->draw();
		
		if (palette)
			palette->draw();

		frames_dialog->draw();
		layers_dialog->draw();
		animations_dialog->draw();

		main_menu->draw();

		for (auto& dialog : dialogs)
			dialog->draw();

		cursor->draw();

		window->display();

		
		
	}

	
}


