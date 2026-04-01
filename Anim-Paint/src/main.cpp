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
#include "WorldToTileConverter.hpp"
#include "History.hpp"
#include "Tools/ClipBoard.hpp"
#include "Tools/Filters.hpp"

#include "Animation/Layer.hpp"
#include "Animation/Frame.hpp"
#include "Animation/Animation.hpp"

#include "Dialogs/Dialog.hpp"
#include "Dialogs/Palette.hpp"

#include "Components/FramesPanel.hpp"
#include "Components/LayersPanel/LayersPanel.hpp"
#include "Components/AnimationsPanel.hpp"
#include "Components/PreviewAnimationPanel.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/BottomBar.hpp"
#include "Components/Tooltip.hpp"
#include "Components/Canvas.hpp"

#include "Tools/Selection.hpp"
#include "Tools/Brush.hpp"



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
		position.y = int(main_menu->getSize().y) + toolbar->_rect.size.y;
		animations_panel->setPosition(position);
	}

	{
		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 192 - dialog_margin;
		position.y = animations_panel->getPosition().y + animations_panel->getSize().y;
		frames_panel->setPosition(position);
	}

	{
		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 160 - dialog_margin;
		position.y = frames_panel->getPosition().y + frames_panel->getSize().y;
		layers_panel->setPosition(position);
	}
	
	{
		sf::Vector2i position;
		position.x = dialog_margin;
		position.y = int(main_menu->getSize().y) + toolbar->_rect.size.y;
		preview_animation_panel->setPosition(position);
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
	animation->setLastLayerAsCurrent();

	toolbar = std::make_shared<Toolbar>();
	
	animations_panel = std::make_shared<AnimationsPanel>();
	frames_panel = std::make_shared<FramesPanel>();
	layers_panel = std::make_shared<LayersPanel>();
	preview_animation_panel = std::make_shared<PreviewAnimationPanel>();

	static_dialogs.push_back(animations_panel);
	static_dialogs.push_back(frames_panel);
	static_dialogs.push_back(layers_panel);
	main_menu->setActiveForWindows();

	canvas = std::make_shared<Canvas>();
	bottom_bar = std::make_shared<BottomBar>();
	tooltip = std::make_shared<Tooltip>();

	brush = std::make_shared<Brush>(2);
	selection = std::make_shared<Selection>();

	history = std::make_shared<History>();
	history->saveStep();

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

		// delete old static dialogs
		bool removeDialog = false;
		for (int i = 0; i < static_dialogs.size(); ) {
			if (static_dialogs[i]->_state == DialogState::ToClose) {
				static_dialogs.erase(static_dialogs.begin() + i);
				removeDialog = true;
			}
			else {
				i++;
			}
		}

		if (removeDialog == true) {
			main_menu->setActiveForWindows();
		}

		if (palette && palette->_state == DialogState::ToClose) {
			static_dialogs.erase(std::remove(static_dialogs.begin(), static_dialogs.end(), palette), static_dialogs.end());
			palette = nullptr;
		}
			

		// calculate the FPS
		frameCount++;

		if (fpsClock.getElapsedTime().asSeconds() >= 1.0f) {
			fps = frameCount / fpsClock.restart().asSeconds();
			frameCount = 0;
			window->setTitle(L"Anim Paint - FPS: " + std::to_wstring(int(fps)));
		}

		// cursor hovering
		Element_hovered = nullptr;

		selection->cursorHover();
		canvas->cursorHover();

		toolbar->cursorHover();
		main_menu->cursorHover();
		bottom_bar->cursorHover();

		for (auto it = static_dialogs.end(); it != static_dialogs.begin();) {
			it--;
			auto& dialog = *it;
			dialog->cursorHover();
		}

		for (auto& dialog : dialogs)
			dialog->cursorHover();

		// Element GUI update before handle events
		for (auto& dialog : dialogs)
			dialog->update();
		toolbar->update();

		
		
		canvas->update();

		for(auto& dialog : static_dialogs)
			dialog->update();

		main_menu->update();
		bottom_bar->update();
		tooltip->update();

		// handle events
		while (const std::optional event = window->pollEvent()) {

			if (event->is<sf::Event::Closed>())
				window->close();

			if (!dialogs.empty())
				dialogs.back()->handleEvent(*event);

			if (const auto* resized = event->getIf<sf::Event::Resized>()) {
				sf::Vector2f newSize(resized->size);
				mainView.setSize(newSize);
				mainView.setCenter(newSize / 2.f);
				window->setView(mainView);
				resize();
			}

			

			history->handleEvent(*event);
			main_menu->handleEvent(*event);

			toolbar->handleEvent(*event);
			selection->handleEvent(*event);
			canvas->handleEvent(*event);
			
			for (auto it = static_dialogs.begin(); it != static_dialogs.end(); it+=1) {
				auto& dialog = *it;

				dialog->handleEvent(*event);
				if (dialog->_clickArea == DialogClickArea::Inside) {
					if (const auto* mp = event->getIf<sf::Event::MouseButtonPressed>(); mp && mp->button == sf::Mouse::Button::Left) {
						auto selected = dialog;
						it = static_dialogs.erase(it);
						static_dialogs.emplace(static_dialogs.begin(), selected);
						
					}
					break;
				}
				
			}
				

			cursor->handleEvent(*event);
			bottom_bar->handleEvent(*event);

		}
		
		// update
		selection->update();
		
		// render
		window->clear(sf::Color(56, 56, 56));
		canvas->draw();
		selection->draw();
		cursor->draw();

		for (auto it = static_dialogs.end(); it != static_dialogs.begin(); ) {
			it--;
			auto& dialog = *it;
			dialog->draw();
		}
			

		toolbar->draw();

		if(!static_dialogs.empty() && static_dialogs.front()->_is_moved)
			static_dialogs.front()->draw();

		

		bottom_bar->draw();
		main_menu->draw();

		for (auto& dialog : dialogs)
			dialog->draw();

		tooltip->draw();
		window->display();
	}

	
}


