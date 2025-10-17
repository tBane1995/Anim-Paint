#include "MainMenu.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Mouse.hpp"
#include "Window.hpp"
#include "Dialogs/Dialog.hpp"
#include "Dialogs/Dialog_Save_As.hpp"
#include "Dialogs/Dialog_Rotation.hpp"
#include "Dialogs/Dialog_Brightness_Contrast.hpp"
#include "Dialogs/Dialog_Saturation.hpp"
#include "Dialogs/Dialog_Sepia.hpp"
#include "Animation/Animation.hpp"
#include <iostream>
#include <filesystem>
#include "Tools/Lasso.hpp"
#include "Tools/Selection.hpp"

OptionBox::OptionBox(std::wstring text) 
: _text(basicFont, text, menu_font_size)
{
	_text.setFillColor(menu_text_color);
	_rect = sf::RectangleShape(sf::Vector2f(_text.getGlobalBounds().size.x + 2 * menu_horizontal_margin, menu_height));
	_rect.setFillColor(optionbox_idle_color);
	_state = ButtonState::Idle;
	_onclick_func = { };
}

OptionBox::~OptionBox() {}

void OptionBox::setPosition(sf::Vector2f position) {
	_rect.setPosition(position);
	_text.setPosition(position + sf::Vector2f(menu_horizontal_margin, (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));
}

void OptionBox::unclick() {
	_state = ButtonState::Idle;
	_rect.setFillColor(optionbox_idle_color);
}

void OptionBox::hover() {
	_state = ButtonState::Hover;
	_rect.setFillColor(optionbox_hover_color);

}

void OptionBox::click() {
	_state = ButtonState::Pressed;
	_rect.setFillColor(optionbox_press_color);
	_clickTime = currentTime;
}


void OptionBox::cursorHover() {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}
}

void OptionBox::handleEvent(const sf::Event& event)
{
	if (!_rect.getGlobalBounds().contains(worldMousePosition))
		return;

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left)
	{
		ElementGUI_pressed = this;
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left)
	{
		if (ElementGUI_pressed == this)
			click();
	}
}

void OptionBox::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
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
	window->draw(_rect);
	window->draw(_text);
}

/////////////////////////////////////////////////////////////////////////
MenuBox::MenuBox(std::wstring text) 
: ElementGUI(), 
_text(basicFont, text, menu_font_size)
{
	_text.setFillColor(menu_text_color);
	_rect = sf::RectangleShape(sf::Vector2f(_text.getGlobalBounds().size.x + 2 * menu_horizontal_margin, menu_height));

	_isOpen = false;
	_options.clear();
}

MenuBox::~MenuBox() {

}

void MenuBox::addOption(OptionBox* option) {
	_options.push_back(option);

	int max_wdt = 0;
	for (auto& o : _options) {
		if (o->_rect.getSize().x > max_wdt)
			max_wdt = o->_rect.getSize().x;
	}

	for (auto& o : _options) {
		o->_rect.setSize(sf::Vector2f(max_wdt, menu_height));
	}


}

void MenuBox::setPosition(sf::Vector2f position) {
	_rect.setPosition(position);
	_text.setPosition(position + sf::Vector2f(menu_horizontal_margin, (menu_height - basicFont.getLineSpacing(menu_font_size)) / 2));

	for (int i = 0; i < _options.size(); i++) {
		_options[i]->setPosition(_rect.getPosition() + sf::Vector2f(0, _rect.getSize().y + i * menu_height));
	}
}

void MenuBox::unclick() {
	_state = ButtonState::Idle;
	if (_isOpen) {
		_rect.setFillColor(menubox_open_color);
	}
	else {
		_rect.setFillColor(menubox_idle_color);
	}
}

void MenuBox::hover() {
	_state = ButtonState::Hover;
	_rect.setFillColor(menubox_hover_color);

}

void MenuBox::click() {
	_state = ButtonState::Pressed;
	_rect.setFillColor(menubox_press_color);
	_clickTime = currentTime;
}


void MenuBox::cursorHover() {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	if (_isOpen) {
		for (auto& option : _options)
			option->cursorHover();
	}
}

void MenuBox::handleEvent(const sf::Event& event) {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this;
		}
		else if (const auto* mbp = event.getIf < sf::Event::MouseButtonReleased > (); mbp && mbp->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed == this) {
				click();
			}
		}

	}

	if (_isOpen) {
		for (auto& option : _options)
			option->handleEvent(event);
	}
}

void MenuBox::update() {

	if (_state == ButtonState::Pressed) {
		if ((currentTime - _clickTime).asSeconds() > 0.05f) {
			if (_onclick_func) {
				_onclick_func();
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


	for (auto& option : _options)
		option->update();
}

void MenuBox::draw() {
	window->draw(_rect);
	window->draw(_text);

	if (_isOpen) {
		for (auto& option : _options)
			option->draw();
	}
}

/////////////////////////////////////////////////////////////////////////

MainMenu::MainMenu() : 
ElementGUI(), 
_logo(*getTexture(L"tex\\logo\\small_logo.png")->_texture) 
{
	_rect = sf::RectangleShape(sf::Vector2f(window->getSize().x, menu_height));
	_rect.setFillColor(menu_bar_color);
	_rect.setPosition(sf::Vector2f(0,0));


	// FILE
	MenuBox* file = new MenuBox(L"file");
	file->_onclick_func = [this, file]() {
		if (_open_menu_box != nullptr) {
			_open_menu_box->_isOpen = false;
		}
			

		_open_menu_box = file;
		_open_menu_box->_isOpen = true;
		};
	_menu_boxes.push_back(file);

	OptionBox* file_new = new OptionBox(L"new file");
	file_new->_onclick_func = [this]() {
		dialogs.push_back(new Dialog(L"new file", sf::Vector2f(200, 200)));
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};
	OptionBox* file_save = new OptionBox(L"save");

	OptionBox* file_saveAs = new OptionBox(L"save as");
	file_saveAs->_onclick_func = [this]() {
		dialogs.push_back(new Dialog_Save_As(
			L"save project as",
			L"save", 
			[this](const std::filesystem::path& target) {
				this->saveProject(target);
			}
			));
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};

	OptionBox* file_load = new OptionBox(L"load");
	file_load->_onclick_func = [this]() {
		dialogs.push_back(new Dialog_Save_As(
			L"load project",
			L"load",
			[this](const std::filesystem::path& target) {
				this->loadProject(target);
			}
		));
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};
	OptionBox* file_export = new OptionBox(L"export");
	file_export->_onclick_func = [this]() {
		dialogs.push_back(new Dialog_Save_As(
			L"export as effects",
			L"export",
			[this](const std::filesystem::path& target) {
				this->exportFile(target);
			}
		));
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};

	OptionBox* file_import = new OptionBox(L"import");
	file_import->_onclick_func = [this]() {
		dialogs.push_back(new Dialog_Save_As(
			L"import from image",
			L"import",
			[this](const std::filesystem::path& target) {
				this->importFile(target);
			}
		));
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};

	file->addOption(file_new);
	file->addOption(file_save);
	file->addOption(file_saveAs);
	file->addOption(file_load);
	file->addOption(file_export);
	file->addOption(file_import);

	// EDIT
	MenuBox* edit = new MenuBox(L"edit");
	edit->_onclick_func = [this, edit]() {
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;

		_open_menu_box = edit;
		_open_menu_box->_isOpen = true;
		};
	_menu_boxes.push_back(edit);

	OptionBox* edit_undo = new OptionBox(L"undo");
	OptionBox* edit_redo = new OptionBox(L"redo");
	OptionBox* edit_cut = new OptionBox(L"cut");
	OptionBox* edit_copy = new OptionBox(L"copy");
	OptionBox* edit_paste = new OptionBox(L"paste");
	OptionBox* edit_paste_as = new OptionBox(L"paste as");

	edit->addOption(edit_undo);
	edit->addOption(edit_redo);
	edit->addOption(edit_cut);
	edit->addOption(edit_copy);
	edit->addOption(edit_paste);
	edit->addOption(edit_paste_as);

	// IMAGE
	MenuBox* effects = new MenuBox(L"effects");
	effects->_onclick_func = [this, effects]() {
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;

		_open_menu_box = effects;
		_open_menu_box->_isOpen = true;
		};
	_menu_boxes.push_back(effects);

	OptionBox* effects_resize_scale = new OptionBox(L"resize/scale");
	OptionBox* effects_trim = new OptionBox(L"trim");
	OptionBox* effects_rotation = new OptionBox(L"rotation");
	effects_rotation->_onclick_func = [this]() {
		dialogs.push_back(new Dialog_Rotation(animation->getLayers()));

		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};
	OptionBox* effects_brightness_contrast = new OptionBox(L"brightness-contrast");
	effects_brightness_contrast->_onclick_func = [this]() {
		dialogs.push_back(new Dialog_Brightness_Contrast(animation->getLayers()));

		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};
	OptionBox* effects_saturation = new OptionBox(L"saturation");
	effects_saturation->_onclick_func = [this]() {
		dialogs.push_back(new Dialog_Saturation(animation->getLayers()));

		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};


	OptionBox* effects_hue = new OptionBox(L"hue");
	OptionBox* effects_gray = new OptionBox(L"grayscale mode");

	OptionBox* effects_sepia = new OptionBox(L"sepia");
	effects_sepia->_onclick_func = [this]() {
		dialogs.push_back(new Dialog_Sepia(animation->getLayers()));

		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;
		_open_menu_box = nullptr;
		};

	OptionBox* effects_invert = new OptionBox(L"invert colors");

	effects->addOption(effects_resize_scale);
	effects->addOption(effects_trim);
	effects->addOption(effects_rotation);
	effects->addOption(effects_brightness_contrast);
	effects->addOption(effects_saturation);
	effects->addOption(effects_hue);
	effects->addOption(effects_gray);
	effects->addOption(effects_sepia);
	effects->addOption(effects_invert);

	// SELECT
	MenuBox* select = new MenuBox(L"select");
	select->_onclick_func = [this, select]() {
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;

		_open_menu_box = select;
		_open_menu_box->_isOpen = true;
		};
	_menu_boxes.push_back(select);

	OptionBox* select_all = new OptionBox(L"select all");
	OptionBox* select_none = new OptionBox(L"none");
	OptionBox* select_invert = new OptionBox(L"invert selection");
	OptionBox* select_align = new OptionBox(L"align center");


	select->addOption(select_all);
	select->addOption(select_none);
	select_none->_onclick_func = [this]() {
		lasso->unselect();
		};
	select->addOption(select_invert);
	select->addOption(select_align);
	select_align->_onclick_func = [this]() {
		
		if (lasso->_image == nullptr)
			return;

		sf::Vector2i newPosition(animation->getCurrentLayer()->_image.getSize()/2u);
		lasso->_outlineOffset = newPosition - lasso->_rect.size/2;
		lasso->generateRect();

		};


	// SETTINGS
	MenuBox* settings = new MenuBox(L"settings");
	settings->_onclick_func = [this, settings]() {
		if (_open_menu_box != nullptr)
			_open_menu_box->_isOpen = false;

		_open_menu_box = settings;
		_open_menu_box->_isOpen = true;
		};
	_menu_boxes.push_back(settings);


	// POSITIONING
	_open_menu_box = nullptr;
	setPosition(sf::Vector2f(0, 0));
}

MainMenu::~MainMenu() {

}

sf::Vector2f MainMenu::getSize() {
	return _rect.getSize();
}

void MainMenu::setPosition(sf::Vector2f position) {
	_rect.setPosition(position);
	_logo.setPosition(sf::Vector2f(0, (menu_height - _logo.getGlobalBounds().size.y) / 2.0f));

	int x = 24;
	int y = position.y + menu_padding;
	for (int i = 0; i < _menu_boxes.size(); i++) {
		_menu_boxes[i]->setPosition(sf::Vector2f(position.x + x, y));
		x = x + _menu_boxes[i]->_rect.getSize().x;
	}
}

void MainMenu::saveProject(const std::filesystem::path& path) {
	

	std::wcout << L"save " << path.wstring() << "\n";
}

void MainMenu::loadProject(const std::filesystem::path& path) {
	std::wcout << "load " << path.wstring() << "\n";
}

void MainMenu::exportFile(const std::filesystem::path& path) {

	sf::RenderTexture tex;
	sf::Vector2f layerSize;
	layerSize.x = animation->getFrame(0)->getLayers()[0]->_image.getSize().x;
	layerSize.y = animation->getFrame(0)->getLayers()[0]->_image.getSize().y;

	tex.resize(sf::Vector2u(layerSize.x * animation->getFramesCount(), layerSize.y));
	tex.clear(sf::Color::Transparent);

	sf::Vector2f offset(0, 0);

	for (int f = 0; f < animation->getFramesCount(); f += 1) {
		for (int l = 0; l < animation->getLayersCount(); l+=1) {
			sf::Texture t;
			t.setSmooth(false);
			t.setRepeated(false);
			t.loadFromImage(animation->getFrame(f)->getLayers()[l]->_image);

			sf::Sprite spr(t);
			spr.setPosition(offset);
			tex.draw(spr);
		}

		offset.x += layerSize.x;
	}

	
	sf::Image finalImage = tex.getTexture().copyToImage();
	finalImage.flipVertically();
	std::wstring filename = path.wstring();
	finalImage.saveToFile(filename);

	std::wcout << "export " << filename << "\n";
}

void MainMenu::importFile(const std::filesystem::path& path) {
	std::wcout << "import " << path.wstring() << "\n";
}

void MainMenu::cursorHover() {
	if (_rect.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

	for (auto& mb : _menu_boxes)
		mb->cursorHover();

}

void MainMenu::handleEvent(const sf::Event& event) {
	bool clicked_in_menu = false;

	for (auto& mb : _menu_boxes) {
		mb->handleEvent(event);
		if (ElementGUI_pressed == mb) {
			clicked_in_menu = true;
		}

		if (mb->_isOpen) {
			for (auto& op : mb->_options) {
				op->handleEvent(event);
				if (ElementGUI_pressed == op) {
					clicked_in_menu = true;
				}
			}
		}
	}

	
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (!clicked_in_menu) {
			if (_open_menu_box != nullptr)
				_open_menu_box->_isOpen = false;
			_open_menu_box = nullptr;
		}
	}
}

void MainMenu::update() {
	for (auto& mb : _menu_boxes)
		mb->update();
}

void MainMenu::draw() {
	window->draw(_rect);
	window->draw(_logo);

	if (_open_menu_box != nullptr) {
		if (_open_menu_box->_options.size() > 0) {
			sf::RectangleShape rect(sf::Vector2f(_open_menu_box->_options.front()->_rect.getSize().x, _open_menu_box->_options.size() * menu_height));
			rect.setPosition(_open_menu_box->_options.front()->_rect.getPosition());
			rect.setOutlineThickness(menuoptions_border_width);
			rect.setOutlineColor(menuoptions_border_color);
			window->draw(rect);
		}
	}

	for (auto& mb : _menu_boxes)
		mb->draw();
}

/////////////////////////////////////////////////////////////////////////

MainMenu* main_menu = nullptr;