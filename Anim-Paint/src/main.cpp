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
#include "History.hpp"
#include "Tools/ClipBoard.hpp"
#include "Tools/Filters.hpp"

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
#include "Dialogs/Dialog_Hue_Saturation.hpp"
#include "Dialogs/Dialog_Sepia.hpp"

#include "MainMenu.hpp"
#include "BottomBar.hpp"

#include "Tooltip.hpp"

#include "Tools/Selection.hpp"
#include "Tools/Lasso.hpp"
#include "Tools/Brush.hpp"
#include "Toolbar.hpp"
#include "Canvas.hpp"

void createDialogs() {
	
	{
		sf::Vector2i size;
		size.x = 192;
		size.y = dialog_title_rect_height + 32 + 32 + dialog_padding * 2;

		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 192 - dialog_margin;
		position.y = int(main_menu->getSize().y) + toolbar->_rect.size.y + dialog_margin;

		animations_dialog = std::make_shared<AnimationsDialog>(L"Animations", size, position);
	}

	{
		sf::Vector2i size;
		size.x = 192;
		size.y = dialog_title_rect_height + 32 + 32 + dialog_padding * 2;
		
		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 192 - dialog_margin;
		position.y = animations_dialog->getPosition().y + animations_dialog->getSize().y + dialog_margin;
		
		frames_dialog = std::make_shared<FramesDialog>(L"Frames", size, position);
	}

	{
		sf::Vector2i size;
		size.x = 160;
		size.y = dialog_title_rect_height + 4 * 32 + 32 + dialog_padding * 2;

		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 160 - dialog_margin;
		position.y = frames_dialog->getPosition().y + frames_dialog->getSize().y + dialog_margin;

		layers_dialog = std::make_shared<LayersDialog>(L"Layers", size, position);
	}

	
}

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
		animations_dialog->setPosition(position);
	}

	{
		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 192 - dialog_margin;
		position.y = animations_dialog->getPosition().y + animations_dialog->getSize().y + dialog_margin;
		frames_dialog->setPosition(position);
	}

	{
		sf::Vector2i position;
		position.x = int(mainView.getSize().x) - 160 - dialog_margin;
		position.y = frames_dialog->getPosition().y + frames_dialog->getSize().y + dialog_margin;
		layers_dialog->setPosition(position);
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

	cursor = new Cursor();
	main_menu = std::make_shared<MainMenu>();
	
	std::shared_ptr<Animation> animation = std::make_shared<Animation>();
	animation->addEmptyFrame(sf::Vector2i(32,32));
	animations.push_back(animation);

	history = std::make_shared<History>();
	history->saveStep();

	selection = std::make_shared<Selection>();
	brush = std::make_shared<Brush>(2);
	toolbar = std::make_shared<Toolbar>();
	createDialogs();
	canvas = std::make_shared<Canvas>();
	bottom_bar = std::make_shared<BottomBar>();
	tooltip = std::make_shared<Tooltip>();

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

		main_menu->cursorHover();

		for (auto& dialog : dialogs)
			dialog->cursorHover();

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

			main_menu->handleEvent(*event);

			canvas->handleEvent(*event);
			toolbar->handleEvent(*event);

			frames_dialog->handleEvent(*event);
			layers_dialog->handleEvent(*event);
			animations_dialog->handleEvent(*event);

			if (palette)
				palette->handleEvent(*event);

			cursor->handleEvent(*event);
			bottom_bar->handleEvent(*event);

			tooltip->handleEvent(*event);
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

		bottom_bar->draw();
		main_menu->draw();

		for (auto& dialog : dialogs)
			dialog->draw();

		tooltip->draw();

		cursor->draw();

		window->display();
	}

	
}


