#include "MainMenu.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Mouse.hpp"
#include "Window.hpp"
#include "Dialogs/Dialog.hpp"
#include "Dialogs/Dialog_Rotation.hpp"
#include "Dialogs/Dialog_Brightness_Contrast.hpp"
#include "Dialogs/Dialog_Saturation.hpp"
#include "Dialogs/Dialog_Sepia.hpp"
#include "Animation/Animation.hpp"

OptionBox::OptionBox(std::wstring text) {
	this->text = sf::Text(text, basicFont, menu_font_size);
	this->text.setFillColor(menu_text_color);
	rect = sf::RectangleShape(sf::Vector2f(this->text.getGlobalBounds().width + 2 * menu_horizontal_margin, menu_height));
	rect.setFillColor(optionbox_idle_color);
	state = ButtonState::Idle;
	onclick_func = { };
}

OptionBox::~OptionBox() {}

void OptionBox::setPosition(sf::Vector2f position) {
	this->rect.setPosition(position);
	this->text.setPosition(position + sf::Vector2f(menu_horizontal_margin, (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
}

void OptionBox::unclick() {
	state = ButtonState::Idle;
	rect.setFillColor(optionbox_idle_color);
}

void OptionBox::hover() {
	state = ButtonState::Hover;
	rect.setFillColor(optionbox_hover_color);

}

void OptionBox::click() {
	state = ButtonState::Pressed;
	rect.setFillColor(optionbox_press_color);
	clickTime = currentTime;
}


void OptionBox::cursorHover() {
	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void OptionBox::handleEvent(sf::Event& event) {
	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			ElementGUI_pressed = this;
		}
		else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
		}

	}
}

void OptionBox::update() {

	if (state == ButtonState::Pressed) {
		if ((currentTime - clickTime).asSeconds() > 0.05f) {
			if (onclick_func) {
				onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();
}

void OptionBox::draw() {
	window->draw(rect);
	window->draw(text);
}

/////////////////////////////////////////////////////////////////////////
MenuBox::MenuBox(std::wstring text) : ElementGUI() {
	this->text = sf::Text(text, basicFont, menu_font_size);
	this->text.setFillColor(menu_text_color);
	rect = sf::RectangleShape(sf::Vector2f(this->text.getGlobalBounds().width + 2 * menu_horizontal_margin, menu_height));

	isOpen = false;
	options.clear();
}

MenuBox::~MenuBox() {

}

void MenuBox::addOption(OptionBox* option) {
	options.push_back(option);

	int max_wdt = 0;
	for (auto& o : options) {
		if (o->rect.getSize().x > max_wdt)
			max_wdt = o->rect.getSize().x;
	}

	for (auto& o : options) {
		o->rect.setSize(sf::Vector2f(max_wdt, menu_height));
	}


}

void MenuBox::setPosition(sf::Vector2f position) {
	this->rect.setPosition(position);
	this->text.setPosition(position + sf::Vector2f(menu_horizontal_margin, (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));

	for (int i = 0; i < options.size(); i++) {
		options[i]->setPosition(this->rect.getPosition() + sf::Vector2f(0, this->rect.getSize().y + i * menu_height));
	}
}

void MenuBox::unclick() {
	state = ButtonState::Idle;
	rect.setFillColor(menubox_idle_color);
}

void MenuBox::hover() {
	state = ButtonState::Hover;
	rect.setFillColor(menubox_hover_color);

}

void MenuBox::click() {
	state = ButtonState::Pressed;
	rect.setFillColor(menubox_press_color);
	clickTime = currentTime;
}


void MenuBox::cursorHover() {
	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	if (isOpen) {
		for (auto& option : options)
			option->cursorHover();
	}
}

void MenuBox::handleEvent(sf::Event& event) {
	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			ElementGUI_pressed = this;
		}
		else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
		}

	}

	if (isOpen) {
		for (auto& option : options)
			option->handleEvent(event);
	}
}

void MenuBox::update() {

	if (state == ButtonState::Pressed) {
		if ((currentTime - clickTime).asSeconds() > 0.05f) {
			if (onclick_func) {
				onclick_func();
			}
			ElementGUI_pressed = nullptr;
			unclick();
		}
	}
	else if (ElementGUI_hovered == this) {
		hover();
	}
	else
		unclick();


	for (auto& option : options)
		option->update();
}

void MenuBox::draw() {
	window->draw(rect);
	window->draw(text);

	if (isOpen) {
		for (auto& option : options)
			option->draw();
	}
}

/////////////////////////////////////////////////////////////////////////

MainMenu::MainMenu() : ElementGUI() {
	rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, menu_height));
	rect.setFillColor(menu_bar_color);
	rect.setPosition(0, 0);

	logo = sf::Sprite(*getTexture(L"tex\\logo\\small_logo.png")->texture);

	// FILE
	MenuBox* file = new MenuBox(L"file");
	file->onclick_func = [this, file]() {
		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;

		open_menu_box = file;
		open_menu_box->isOpen = true;
		};
	menu_boxes.push_back(file);

	OptionBox* file_new = new OptionBox(L"new file");
	file_new->onclick_func = [this]() {
		dialogs.push_back(new Dialog(L"new file", sf::Vector2f(200, 200)));
		if (this->open_menu_box != nullptr)
			open_menu_box->isOpen = false;
		open_menu_box = nullptr;
		};
	OptionBox* file_save = new OptionBox(L"save");
	OptionBox* file_saveAs = new OptionBox(L"save as");
	OptionBox* file_open = new OptionBox(L"open");
	OptionBox* file_export = new OptionBox(L"export");
	OptionBox* file_import = new OptionBox(L"import");

	file->addOption(file_new);
	file->addOption(file_save);
	file->addOption(file_saveAs);
	file->addOption(file_open);
	file->addOption(file_export);
	file->addOption(file_import);

	// EDIT
	MenuBox* edit = new MenuBox(L"edit");
	edit->onclick_func = [this, edit]() {
		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;

		open_menu_box = edit;
		open_menu_box->isOpen = true;
		};
	menu_boxes.push_back(edit);

	OptionBox* edit_undo = new OptionBox(L"undo");
	OptionBox* edit_redo = new OptionBox(L"redo");
	OptionBox* edit_cut = new OptionBox(L"cut");
	OptionBox* edit_copy = new OptionBox(L"copy");
	OptionBox* edit_paste = new OptionBox(L"paste");

	edit->addOption(edit_undo);
	edit->addOption(edit_redo);
	edit->addOption(edit_cut);
	edit->addOption(edit_copy);
	edit->addOption(edit_paste);

	// IMAGE
	MenuBox* image = new MenuBox(L"image");
	image->onclick_func = [this, image]() {
		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;

		open_menu_box = image;
		open_menu_box->isOpen = true;
		};
	menu_boxes.push_back(image);

	OptionBox* image_resize_scale = new OptionBox(L"resize/scale");
	OptionBox* image_trim = new OptionBox(L"trim");
	OptionBox* image_rotation = new OptionBox(L"rotation");
	image_rotation->onclick_func = [this]() {
		dialogs.push_back(new Dialog_Rotation(animation->getLayers()));

		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;
		open_menu_box = nullptr;
		};
	OptionBox* image_brightness_contrast = new OptionBox(L"brightness-contrast");
	image_brightness_contrast->onclick_func = [this]() {
		dialogs.push_back(new Dialog_Brightness_Contrast(animation->getLayers()));

		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;
		open_menu_box = nullptr;
		};
	OptionBox* image_saturation = new OptionBox(L"saturation");
	image_saturation->onclick_func = [this]() {
		dialogs.push_back(new Dialog_Saturation(animation->getLayers()));

		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;
		open_menu_box = nullptr;
		};


	OptionBox* image_hue = new OptionBox(L"hue");
	OptionBox* image_gray = new OptionBox(L"grayscale mode");

	OptionBox* image_sepia = new OptionBox(L"sepia");
	image_sepia->onclick_func = [this]() {
		dialogs.push_back(new Dialog_Sepia(animation->getLayers()));

		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;
		open_menu_box = nullptr;
		};

	OptionBox* image_invert = new OptionBox(L"invert colors");

	image->addOption(image_resize_scale);
	image->addOption(image_trim);
	image->addOption(image_rotation);
	image->addOption(image_brightness_contrast);
	image->addOption(image_saturation);
	image->addOption(image_hue);
	image->addOption(image_gray);
	image->addOption(image_sepia);
	image->addOption(image_invert);

	// SELECT
	MenuBox* select = new MenuBox(L"select");
	select->onclick_func = [this, select]() {
		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;

		open_menu_box = select;
		open_menu_box->isOpen = true;
		};
	menu_boxes.push_back(select);

	OptionBox* select_all = new OptionBox(L"select all");
	OptionBox* select_none = new OptionBox(L"none");
	OptionBox* select_invert = new OptionBox(L"invert selection");
	OptionBox* select_resize_scale = new OptionBox(L"resize/scale");
	OptionBox* select_brightness_contrast = new OptionBox(L"brightness-contrast");
	OptionBox* select_saturation = new OptionBox(L"saturation");
	OptionBox* select_hue = new OptionBox(L"hue");
	OptionBox* select_gray = new OptionBox(L"grayscale mode");
	OptionBox* select_invert_colors = new OptionBox(L"invert colors");


	select->addOption(select_all);
	select->addOption(select_none);
	select->addOption(select_invert);
	select->addOption(select_resize_scale);
	select->addOption(select_brightness_contrast);
	select->addOption(select_saturation);
	select->addOption(select_hue);
	select->addOption(select_gray);
	select->addOption(select_invert_colors);


	// SETTINGS
	MenuBox* settings = new MenuBox(L"settings");
	settings->onclick_func = [this, settings]() {
		if (open_menu_box != nullptr)
			open_menu_box->isOpen = false;

		open_menu_box = settings;
		open_menu_box->isOpen = true;
		};
	menu_boxes.push_back(settings);


	// POSITIONING
	open_menu_box = nullptr;
	setPosition(sf::Vector2f(0, 0));
}

MainMenu::~MainMenu() {

}

sf::Vector2f MainMenu::getSize() {
	return rect.getSize();
}

void MainMenu::setPosition(sf::Vector2f position) {
	rect.setPosition(position);
	logo.setPosition(sf::Vector2f(0, (menu_height - logo.getGlobalBounds().getSize().y) / 2.0f));

	int x = 24;
	int y = position.y + menu_padding;
	for (int i = 0; i < menu_boxes.size(); i++) {
		menu_boxes[i]->setPosition(sf::Vector2f(position.x + x, y));
		x = x + menu_boxes[i]->rect.getSize().x;
	}
}

void MainMenu::cursorHover() {
	if (rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	for (auto& mb : menu_boxes)
		mb->cursorHover();

}

void MainMenu::handleEvent(sf::Event& event) {
	bool clicked_in_menu = false;

	for (auto& mb : menu_boxes) {
		mb->handleEvent(event);
		if (ElementGUI_pressed == mb) {
			clicked_in_menu = true;
		}

		if (mb->isOpen) {
			for (auto& op : mb->options) {
				op->handleEvent(event);
				if (ElementGUI_pressed == op) {
					clicked_in_menu = true;
				}
			}
		}
	}

	if (event.type == sf::Event::MouseButtonPressed &&
		event.mouseButton.button == sf::Mouse::Left) {
		if (!clicked_in_menu) {
			if (open_menu_box != nullptr)
				open_menu_box->isOpen = false;
			open_menu_box = nullptr;
		}
	}
}

void MainMenu::update() {
	for (auto& mb : menu_boxes)
		mb->update();
}

void MainMenu::draw() {
	window->draw(rect);
	window->draw(logo);

	if (open_menu_box != nullptr) {
		if (open_menu_box->options.size() > 0) {
			sf::RectangleShape rect(sf::Vector2f(open_menu_box->options.front()->rect.getSize().x, open_menu_box->options.size() * menu_height));
			rect.setPosition(open_menu_box->options.front()->rect.getPosition());
			rect.setOutlineThickness(menuoptions_border_width);
			rect.setOutlineColor(menuoptions_border_color);
			window->draw(rect);
		}
	}

	for (auto& mb : menu_boxes)
		mb->draw();
}

/////////////////////////////////////////////////////////////////////////

MainMenu* main_menu = nullptr;