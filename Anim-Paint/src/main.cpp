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

#include "Element.hpp"
#include "Controls/Button.hpp"
#include "Controls/Checkbox.hpp"
#include "Controls/Slider.hpp"



#include "WorldToTileConverter.hpp"
#include "History.hpp"
#include "Tools/ClipBoard.hpp"
#include "Tools/Filters.hpp"

#include "Animation/Layer.hpp"
#include "Animation/Frame.hpp"
#include "Animation/Animation.hpp"

#include "Dialogs/Dialog.hpp"
#include "Components/FramesPanel.hpp"
#include "Components/LayersPanel/LayersPanel.hpp"
#include "Components/AnimationsPanel.hpp"
#include "Dialogs/Palette.hpp"
#include "Dialogs/FileDialog/FileDialog.hpp"
#include "Dialogs/Dialog_Rotation.hpp"
#include "Dialogs/Dialog_Brightness_Contrast.hpp"
#include "Dialogs/Dialog_Hue_Saturation.hpp"
#include "Dialogs/Dialog_Sepia.hpp"

#include "Components/MainMenu/MainMenu.hpp"
#include "Components/BottomBar.hpp"

#include "Components/Tooltip.hpp"

#include "Tools/Selection.hpp"
#include "Tools/Brush.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"

void resize() {

	main_menu->resize();
	bottom_bar->resize();
	canvas->setCenter();

	if (palette != nullptr) {
		palette->clampPosition();
	}
	
	{
		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 192 - dialog_margin;
		position.y = int(main_menu->getSize().y) + toolbar->_rect.size.y + dialog_margin;
		animations_panel->setPosition(position);
	}

	{
		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 192 - dialog_margin;
		position.y = animations_panel->getPosition().y + animations_panel->getSize().y + dialog_margin;
		frames_panel->setPosition(position);
	}

	{
		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 160 - dialog_margin;
		position.y = frames_panel->getPosition().y + frames_panel->getSize().y + dialog_margin;
		layers_panel->setPosition(position);
	}
	
}

int main() {
	(void)_setmode(_fileno(stdout), _O_U16TEXT); // wide char UTF-16 output
	window = std::make_unique<sf::RenderWindow>(sf::VideoMode(sf::Vector2u(mainView.getSize())), "Anim Paint", sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close);
	
	//window->setKeyRepeatEnabled(false);
	loadTextures();
	loadTheme();

	//window->setFramerateLimit(120);
	window->setView(mainView);
	window->setMinimumSize(sf::Vector2u(mainView.getSize()));

	cursor = std::make_shared<Cursor>();
	main_menu = std::make_shared<MainMenu>();
	
	std::shared_ptr<Animation> animation = std::make_shared<Animation>();
	animation->addEmptyFrame(sf::Vector2i(32,32));
	animations.push_back(animation);

	history = std::make_shared<History>();
	history->saveStep();

	
	toolbar = std::make_shared<Toolbar>();
	animations_panel = std::make_shared<AnimationsPanel>();
	frames_panel = std::make_shared<FramesPanel>();
	layers_panel = std::make_shared<LayersPanel>();
	canvas = std::make_shared<Canvas>();
	bottom_bar = std::make_shared<BottomBar>();
	tooltip = std::make_shared<Tooltip>();

	
	brush = std::make_shared<Brush>(2);
	selection = std::make_shared<Selection>();

	// FPS counter
	sf::Clock fpsClock;
	float fps = 0.0f;
	int frameCount = 0;

	// Elements GUI
	Element_hovered = nullptr;
	Element_pressed = nullptr;

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
		Element_hovered = nullptr;

		canvas->cursorHover();
		

		toolbar->cursorHover();
		frames_panel->cursorHover();
		layers_panel->cursorHover();
		animations_panel->cursorHover();

		if (palette)
			palette->cursorHover();

		main_menu->cursorHover();
		selection->cursorHover();

		for (auto& dialog : dialogs)
			dialog->cursorHover();

		// Element GUI update before handle events
		for (auto& dialog : dialogs)
			dialog->update();
		toolbar->update();

		
		
		canvas->update();
		frames_panel->update();
		layers_panel->update();
		animations_panel->update();
		if (palette)
			palette->update();
		main_menu->update();
		bottom_bar->update();
		tooltip->update();

		// handle events
		while (const std::optional event = window->pollEvent()) {

			if (event->is<sf::Event::Closed>())
				window->close();

			for (auto& dialog : dialogs) {
				dialog->handleEvent(*event);
			}

			if (const auto* resized = event->getIf<sf::Event::Resized>()) {
				sf::Vector2f newSize(resized->size);
				mainView.setSize(newSize);
				mainView.setCenter(newSize / 2.f);
				window->setView(mainView);
				resize();
			}

			history->handleEvent(*event);
			selection->handleEvent(*event);

			main_menu->handleEvent(*event);

			if (palette)
				palette->handleEvent(*event);

			canvas->handleEvent(*event);
			toolbar->handleEvent(*event);

			frames_panel->handleEvent(*event);
			layers_panel->handleEvent(*event);
			animations_panel->handleEvent(*event);

			

			cursor->handleEvent(*event);
			bottom_bar->handleEvent(*event);

		}
		
		// update
		selection->update();
		
		// render
		window->clear(sf::Color(56, 56, 56));
		canvas->draw();
		toolbar->draw();
		
		if (palette)
			palette->draw();

		frames_panel->draw();
		layers_panel->draw();
		animations_panel->draw();

		bottom_bar->draw();
		main_menu->draw();

		for (auto& dialog : dialogs)
			dialog->draw();

		selection->draw(toolbar->_second_color->_color);
		tooltip->draw();

		cursor->draw();

		window->display();
	}

	
}


