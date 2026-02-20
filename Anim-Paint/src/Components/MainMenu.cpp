#include "Components/MainMenu.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

#include "Dialogs/Dialog.hpp"
#include "Dialogs/FileDialog.hpp"
#include "Dialogs/Dialog_Save_Project.hpp"
#include "Dialogs/Dialog_Load_Project.hpp"
#include "Dialogs/Dialog_Import.hpp"
#include "Dialogs/Dialog_Export.hpp"

#include "Dialogs/Dialog_Rotation.hpp"
#include "Dialogs/Dialog_Brightness_Contrast.hpp"
#include "Dialogs/Dialog_Hue_Saturation.hpp"
#include "Dialogs/Dialog_Sepia.hpp"
#include "Dialogs/Dialog_Outline.hpp"
#include "Dialogs/Dialog_Resize.hpp"
#include "Dialogs/Dialog_Chessboard.hpp"
#include "Dialogs/Dialog_Invert_Colors.hpp"
#include "Dialogs/Dialog_Smoothing.hpp"

#include "Dialogs/FramesDialog.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Animation/Animation.hpp"
#include <iostream>
#include <filesystem>
#include "Tools/Lasso.hpp"
#include "Tools/Selection.hpp"
#include "Components/Canvas.hpp"
#include "History.hpp"
#include <fstream>
#include <DebugLog.hpp>

OptionBox::OptionBox(std::wstring text) : ElementGUI() {
	_text = std::make_unique<sf::Text>(basicFont, text, menu_font_size);
	_text->setFillColor(menu_text_color);

	sf::Vector2f rectSize;
	rectSize.x = _text->getGlobalBounds().size.x + (float)(2 * menu_horizontal_margin);
	rectSize.y = (float)menu_height;

	_rect = sf::RectangleShape(rectSize);
	_rect.setFillColor(optionbox_idle_color);

	_state = ButtonState::Idle;
	_onclick_func = { };
}

OptionBox::~OptionBox() {

}

void OptionBox::setPosition(sf::Vector2i position) {
	_rect.setPosition(sf::Vector2f(position));
	
	sf::Vector2f textPos;
	textPos.x = (float)(position.x + menu_horizontal_margin);
	textPos.y = (float)(position.y) + ((float)menu_height - basicFont.getLineSpacing(menu_font_size)) / 2.0f;
	_text->setPosition(textPos);
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
	if (_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_worldMousePosition))) {
		ElementGUI_hovered = this->shared_from_this();
	}
}

void OptionBox::handleEvent(const sf::Event& event)
{
	if (!_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_worldMousePosition)))
		return;

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left)
	{
		ElementGUI_pressed = this->shared_from_this();
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left)
	{
		if (ElementGUI_pressed.get() == this)
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
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();
}

void OptionBox::draw() {
	window->draw(_rect);
	window->draw(*_text);
}

/////////////////////////////////////////////////////////////////////////
MenuBox::MenuBox(std::wstring text) : ElementGUI() {

	_text = std::make_unique<sf::Text>(basicFont, text, menu_font_size);
	_text->setFillColor(menu_text_color);

	sf::Vector2f rectSize;
	rectSize.x = _text->getGlobalBounds().size.x + (float)(2 * menu_horizontal_margin);
	rectSize.y = (float)(menu_height);
	_rect = sf::RectangleShape(rectSize);

	_isOpen = false;
	_options.clear();
}

MenuBox::~MenuBox() {

}

void MenuBox::addOption(std::shared_ptr<OptionBox> option) {
	_options.push_back(option);

	int max_wdt = 0;
	for (auto& o : _options) {
		if ((int)(o->_rect.getSize().x) > max_wdt)
			max_wdt = (int)(o->_rect.getSize().x);
	}

	for (auto& o : _options) {
		sf::Vector2f size;
		size.x = (float)(max_wdt);
		size.y = (float)(menu_height);
		o->_rect.setSize(size);
	}


}

void MenuBox::setPosition(sf::Vector2i position) {
	_rect.setPosition(sf::Vector2f(position));

	sf::Vector2f textPos;
	textPos.x = (float)(position.x + menu_horizontal_margin);
	textPos.y = (float)(position.y) + ((float)(menu_height) - basicFont.getLineSpacing(menu_font_size)) / 2.0f;
	_text->setPosition(textPos);

	for (int i = 0; i < _options.size(); i++) {
		sf::Vector2i optionPos;
		optionPos.x = (int)(_rect.getPosition().x);
		optionPos.y = (int)(_rect.getPosition().y + _rect.getSize().y) + i * menu_height;
		_options[i]->setPosition(optionPos);
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
	if (_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_worldMousePosition))) {
		ElementGUI_hovered = this->shared_from_this();
	}

	if (_isOpen) {
		for (auto& option : _options)
			option->cursorHover();
	}
}

void MenuBox::handleEvent(const sf::Event& event) {
	if (_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_worldMousePosition))) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
		else if (const auto* mbp = event.getIf < sf::Event::MouseButtonReleased > (); mbp && mbp->button == sf::Mouse::Button::Left) {
			if (ElementGUI_pressed.get() == this) {
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
	else if (ElementGUI_hovered.get() == this) {
		hover();
	}
	else
		unclick();


	for (auto& option : _options)
		option->update();
}

void MenuBox::draw() {
	window->draw(_rect);
	window->draw(*_text);

	if (_isOpen) {
		for (auto& option : _options)
			option->draw();
	}
}

/////////////////////////////////////////////////////////////////////////

MainMenu::MainMenu() : ElementGUI() {

	sf::Vector2f rectSize;
	rectSize.x = (float)(window->getSize().x);
	rectSize.y = (float)(menu_height);

	_rect = sf::RectangleShape(rectSize);
	_rect.setFillColor(menu_bar_color);
	_rect.setPosition(sf::Vector2f(0,0));

	_logo = std::make_shared<sf::Sprite>(*getTexture(L"tex\\logo\\small_logo.png")->_texture);

	// FILE
	std::shared_ptr<MenuBox> file = std::make_shared<MenuBox>(L"file");
	file->_onclick_func = [this, file]() {
		hideMenu();
		openMenuBox(file);
		};
	_menu_boxes.push_back(file);

	std::shared_ptr<OptionBox> file_new = std::make_shared<OptionBox>(L"new file");
	file_new->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog>(L"new file", sf::Vector2i(200, 200)));
		closeMenu();
		};
	std::shared_ptr<OptionBox> file_save = std::make_shared<OptionBox>(L"save");

	std::shared_ptr<OptionBox> file_saveAs = std::make_shared<OptionBox>(L"save as");
	file_saveAs->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Save_Project>());
		closeMenu();
		};

	std::shared_ptr<OptionBox> file_load = std::make_shared<OptionBox>(L"load");
	file_load->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Load_Project>());
		closeMenu();
		};
	std::shared_ptr<OptionBox> file_export = std::make_shared<OptionBox>(L"export");
	file_export->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Export>());
		closeMenu();
		};

	std::shared_ptr<OptionBox> file_import = std::make_shared<OptionBox>(L"import");
	file_import->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Import>());
		closeMenu();
		};

	file->addOption(file_new);
	file->addOption(file_save);
	file->addOption(file_saveAs);
	file->addOption(file_load);
	file->addOption(file_export);
	file->addOption(file_import);

	// EDIT
	std::shared_ptr<MenuBox> edit = std::make_shared<MenuBox>(L"edit");
	edit->_onclick_func = [this, edit]() {
		hideMenu();
		openMenuBox(edit);
		};
	_menu_boxes.push_back(edit);

	std::shared_ptr<OptionBox> edit_undo = std::make_shared<OptionBox>(L"undo");
	edit_undo->_onclick_func = [this]() {
		history->undo();
		//closeMenu();
		};

	std::shared_ptr<OptionBox> edit_redo = std::make_shared<OptionBox>(L"redo");
	edit_redo->_onclick_func = [this]() {
		history->redo();
		//closeMenu();
		};

	std::shared_ptr<OptionBox> edit_cut = std::make_shared<OptionBox>(L"cut");
	std::shared_ptr<OptionBox> edit_copy = std::make_shared<OptionBox>(L"copy");
	std::shared_ptr<OptionBox> edit_paste = std::make_shared<OptionBox>(L"paste");
	std::shared_ptr<OptionBox> edit_paste_as = std::make_shared<OptionBox>(L"paste as");

	edit->addOption(edit_undo);
	edit->addOption(edit_redo);
	edit->addOption(edit_cut);
	edit->addOption(edit_copy);
	edit->addOption(edit_paste);
	edit->addOption(edit_paste_as);

	// TOOLS
	std::shared_ptr<MenuBox> tools = std::make_shared<MenuBox>(L"tools");
	tools->_onclick_func = [this, tools]() {
		hideMenu();
		openMenuBox(tools);
		};
	_menu_boxes.push_back(tools);

	std::shared_ptr<OptionBox> tools_resize = std::make_shared<OptionBox>(L"resize");
	tools_resize->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Resize>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	std::shared_ptr<OptionBox> tools_rotation = std::make_shared<OptionBox>(L"rotation");
	tools_rotation->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Rotation>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	std::shared_ptr<OptionBox> tools_brightness_contrast = std::make_shared<OptionBox>(L"brightness-contrast");
	tools_brightness_contrast->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Brightness_Contrast>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	std::shared_ptr<OptionBox> tools_hue_saturation = std::make_shared<OptionBox>(L"hue-saturation");
	tools_hue_saturation->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Hue_Saturation>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	std::shared_ptr<OptionBox> tools_gray = std::make_shared<OptionBox>(L"grayscale mode");

	std::shared_ptr<OptionBox> tools_sepia = std::make_shared<OptionBox>(L"sepia");
	tools_sepia->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Sepia>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	std::shared_ptr<OptionBox> tools_outline = std::make_shared<OptionBox>(L"outline");
	tools_outline->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Outline>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	

	std::shared_ptr<OptionBox> tools_invert = std::make_shared<OptionBox>(L"invert colors");
	tools_invert->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Invert_Colors>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	std::shared_ptr<OptionBox> tools_chessboard = std::make_shared<OptionBox>(L"chessboard");
	tools_chessboard->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Chessboard>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	std::shared_ptr<OptionBox> tools_smoothing = std::make_shared<OptionBox>(L"smoothing");
	tools_smoothing->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Smoothing>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools->addOption(tools_resize);
	tools->addOption(tools_rotation);
	tools->addOption(tools_brightness_contrast);
	tools->addOption(tools_hue_saturation);
	tools->addOption(tools_gray);
	tools->addOption(tools_sepia);
	tools->addOption(tools_outline);
	tools->addOption(tools_invert);
	tools->addOption(tools_chessboard);
	tools->addOption(tools_smoothing);

	// SELECT
	std::shared_ptr<MenuBox> select = std::make_shared<MenuBox>(L"select");
	select->_onclick_func = [this, select]() {
		hideMenu();
		openMenuBox(select);
		};
	_menu_boxes.push_back(select);

	std::shared_ptr<OptionBox> select_all = std::make_shared<OptionBox>(L"select all");
	select_all->_onclick_func = [this]() {
		toolbar->selectToolButton(toolbar->_btn_lasso);
		toolbar->_toolType = ToolType::Lasso;
		selection->selectAll();
		closeMenu();
		};

	std::shared_ptr<OptionBox> select_none = std::make_shared<OptionBox>(L"none");
	select_none->_onclick_func = [this]() {
		selection->unselect();
		closeMenu();
		};

	std::shared_ptr<OptionBox> select_invert = std::make_shared<OptionBox>(L"invert selection");

	std::shared_ptr<OptionBox> select_align_center = std::make_shared<OptionBox>(L"align center");
	select_align_center->_onclick_func = [this, select]() {
		sf::Vector2i oldPos = selection->_rect.position;
		selection->_rect.position = (canvas->_size - selection->_rect.size) / 2;
		selection->_resizedRect.position = selection->_rect.position;
		selection->_outlineOffset = selection->_rect.position - oldPos + selection->_outlineOffset;
		selection->generateEdgePoints();
		closeMenu();
		};

	select->addOption(select_all);
	select->addOption(select_none);
	select->addOption(select_invert);
	select->addOption(select_align_center);

	// SETTINGS
	std::shared_ptr<MenuBox> settings = std::make_shared<MenuBox>(L"settings");
	settings->_onclick_func = [this, settings]() {
		hideMenu();
		openMenuBox(settings);
		};
	_menu_boxes.push_back(settings);


	// POSITIONING
	_state = MainMenuStates::Closed;
	_open_menu_box = nullptr;
	setPosition(sf::Vector2i(0, 0));
}

MainMenu::~MainMenu() {

}

void MainMenu::resize() {
	sf::Vector2f rectSize;
	rectSize.x = mainView.getSize().x;
	rectSize.y = (float)(menu_height);

	_rect.setSize(rectSize);
	_rect.setPosition(sf::Vector2f(0, 0));
}

sf::Vector2f MainMenu::getSize() {
	return _rect.getSize();
}

void MainMenu::setPosition(sf::Vector2i position) {
	_rect.setPosition(sf::Vector2f(position));
	_logo->setPosition(sf::Vector2f(0, ((float)(menu_height) - _logo->getGlobalBounds().size.y) / 2.0f));

	int x = 24;
	int y = position.y + menu_padding;
	for (int i = 0; i < _menu_boxes.size(); i++) {
		_menu_boxes[i]->setPosition(sf::Vector2i(position.x + x, y));
		x = x + (int)(_menu_boxes[i]->_rect.getSize().x);
	}
}

void MainMenu::hideMenu() {
	if (_open_menu_box != nullptr)
		_open_menu_box->_isOpen = false;

	_state = MainMenuStates::Closing;
	_open_menu_box = nullptr;
}

void MainMenu::closeMenu() {
	if (_open_menu_box != nullptr)
		_open_menu_box->_isOpen = false;

	_state = MainMenuStates::Closed;
	_open_menu_box = nullptr;
}


void MainMenu::openMenuBox(std::shared_ptr<MenuBox> menuBox) {
	_state = MainMenuStates::Opened;
	_open_menu_box = menuBox;
	_open_menu_box->_isOpen = true;
}

void MainMenu::saveProject(const std::filesystem::path& path) {
	
	class Saver {
	public:
		std::ostream& file;

		Saver(std::ostream& file) : file(file) { }

		void save_string(const std::string& str) {
			uint16_t str_len = static_cast <uint16_t>(str.size());
			file.write(reinterpret_cast<const char*>(&str_len), sizeof(str_len));
			file.write(str.data(), str_len);
		}

		void save_wstring(const std::wstring& wstr) {
			uint16_t wstr_len = static_cast<uint16_t>(wstr.size());
			file.write(reinterpret_cast<const char*>(&wstr_len), sizeof(wstr_len));
			file.write(reinterpret_cast<const char*>(wstr.data()), wstr_len * sizeof(wchar_t));
		}

		void save_uint32(uint32_t val) {
			file.write(reinterpret_cast<const char*>(&val), sizeof(uint32_t));
		}

		void save_image(const sf::Image& image) {

			sf::Vector2u size = image.getSize();

			save_uint32(size.x);
			save_uint32(size.y);

			auto encoded = image.saveToMemory("png");
			if (!encoded) {
				uint32_t zero = 0;
				save_uint32(zero);
				return;
			}

			const auto& data = *encoded;
			save_uint32(static_cast<uint32_t>(data.size()));
			file.write(reinterpret_cast<const char*>(data.data()), data.size());
		}
	};

	std::ofstream file(path, std::ios::binary);
	Saver saver(file);

	saver.save_uint32(getAnimationsCount());
	for (auto& anim : animations) {
		saver.save_uint32(anim->getFramesCount());
		for(auto& frame : anim->getFrames()) {
			saver.save_uint32(frame->getLayersCount());
			for (auto& layer : frame->getLayers()) {
				saver.save_wstring(layer->_name);
				saver.save_image(layer->_image);
			}
		}
	}

	file.close();
	
	std::wcout << L"save " << path.wstring() << "\n";
}

void MainMenu::loadProject(const std::filesystem::path& path) {
	//std::wcout << "load " << path.wstring() << "\n";

	class Reader {
		std::istream& is;

	public:
		Reader(std::istream& is) : is(is) {}

		std::string read_string() {
			uint16_t len;
			is.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
			std::string str(len, '\0'); // utwórz pusty string o długości len
			is.read(&str[0], len);      // bezpieczne wczytanie danych do stringa
			return str;
		}

		std::wstring read_wstring() {
			uint16_t len;
			is.read(reinterpret_cast<char*>(&len), sizeof(uint16_t));
			std::wstring wstr(len, L'\0'); // utwórz pusty wstring o długości len
			is.read(reinterpret_cast<char*>(&wstr[0]), len * sizeof(wchar_t)); // bezpieczne wczytanie danych do wstringa
			//std::wcout << L"read_wstring: len=" << len << L" wstr=\"" << wstr << L"\"\n";
			return wstr;
		}

		uint32_t read_uint32() {
			uint32_t val;
			is.read(reinterpret_cast<char*>(&val), sizeof(uint32_t));
			return val;
		}

		sf::Image read_image() {
			sf::Image image;

			uint32_t width = read_uint32();
			uint32_t height = read_uint32();
			uint32_t data_size = read_uint32();

			//std::wcout << L"read_image: width=" << width << L" height=" << height << L" data_size=" << data_size << L"\n";

			// plik może zawierać mniej danych niż zadeklarowano w data_size
			std::streampos cur = is.tellg();
			is.seekg(0, std::ios::end);
			std::streampos end = is.tellg();
			is.seekg(cur);

			const std::uint64_t remaining =
				static_cast<std::uint64_t>(end - cur);

			if (data_size == 0) {
				//std::wcout << L"read_image: empty image (data_size == 0)\n";
				return image; // pusty
			}

			if (data_size > remaining) {
				//std::wcout << L"read_image: data_size > remaining bytes in file! " << L"data_size=" << data_size << L" remaining=" << remaining << L"\n";
				// plik jest uszkodzony / format nie pasuje do tego loadera
				return sf::Image();
			}

			std::vector<std::uint8_t> data(data_size);
			is.read(reinterpret_cast<char*>(data.data()),
				static_cast<std::streamsize>(data_size));

			if (!is) {
				//std::wcout << L"read_image: read() failed mimo że remaining wystarczało\n";
				return sf::Image();
			}

			if (!image.loadFromMemory(data.data(), data.size())) {
				//std::wcout << L"read_image: loadFromMemory() failed\n";
				return sf::Image();
			}

			auto loadedSize = image.getSize();
			if (loadedSize.x != width || loadedSize.y != height) {
				//std::wcout << L"read_image: size mismatch, saved=" << width << L"x" << height << L" loaded=" << loadedSize.x << L"x" << loadedSize.y << L"\n";
			}

			return image;
		}
	};

	std::ifstream file(path, std::ios::binary);
	Reader reader(file);
	/*
	saver.save_uint32(getAnimationsCount());
	for (auto& anim : animations) {
		saver.save_uint32(anim->getFramesCount());
		for(auto& frame : anim->getFrames()) {
			saver.save_uint32(frame->getLayersCount());
			for (auto& layer : frame->getLayers()) {
				saver.save_wstring(layer->_name);
				saver.save_image(layer->_image);
			}
		}
	}
	*/

	uint32_t animations_count = reader.read_uint32();
	std::vector<std::shared_ptr<Animation>> new_animations;
	//std::wcout << L"loadProject: animations_count=" << animations_count << L"\n";
	for (uint32_t a = 0; a < animations_count; a++) {

		std::shared_ptr<Animation> animation = std::make_shared<Animation>();
		uint32_t frames_count = reader.read_uint32();
		//std::wcout << L" loadProject: animation " << a << L" frames_count=" << frames_count << L"\n";
		for (uint32_t f = 0; f < frames_count; f++) {
			
			std::shared_ptr<Frame> frame = std::make_shared<Frame>();
			uint32_t layers_count = reader.read_uint32();
			//std::wcout << L"  loadProject: frame " << f << L" layers_count=" << layers_count << L"\n";
			for (uint32_t l = 0; l < layers_count; l++) {

				std::wstring layer_name = reader.read_wstring();
				sf::Image image = reader.read_image();
				std::shared_ptr<Layer> layer = std::make_shared<Layer>(layer_name, image);
				frame->addLayer(layer);
			}
			animation->addFrame(frame);
		}
		new_animations.push_back(animation);
	}

	animations = new_animations;
	file.close();
}

void MainMenu::exportAsFile(const std::filesystem::path& path) {

	sf::RenderTexture tex;

	sf::Vector2i layerSize;
	layerSize.x = canvas->_size.x;
	layerSize.y = canvas->_size.y;

	sf::Vector2u texSize;
	texSize.x = unsigned int(layerSize.x * getCurrentAnimation()->getFramesCount());
	texSize.y = unsigned int(layerSize.y * (int)(animations.size()));
	
	if (!tex.resize(texSize)) {
		DebugError(L"MainMenu::exportAsFile: failed to resize render texture.");
		exit(0);
	}

	tex.clear(sf::Color::Transparent);

	sf::Vector2f offset(0, 0);

	for (int a = 0; a < animations.size(); a++) {

		std::shared_ptr<Animation> anim = getAnimation(a);
		if (anim == nullptr) {
			DebugError(L"MainMenu::exportAsFile: failed to load animation.");
			exit(0);
		}

		for (int f = 0; f < anim->getFramesCount(); f += 1) {

			std::shared_ptr<Frame> frame = anim->getFrame(f);
			if (frame == nullptr) {
				DebugError(L"MainMenu::exportAsFile: failed to load frame.");
				exit(0);
			}

			std::vector<std::shared_ptr<Layer>> layers = frame->getLayers();
			if (layers.empty()) {
				DebugError(L"MainMenu::exportAsFile: failed to load layers.");
				exit(0);
			}

			for (int l = 0; l < (int)(layers.size()); l += 1) {
				sf::Texture t;
				t.setSmooth(false);
				t.setRepeated(false);

				if (!t.loadFromImage(layers[l]->_image)) {
					DebugError(L"MainMenu::exportAsFile: failed to load texture from image.");
					exit(0);
				}

				sf::Sprite spr(t);
				spr.setPosition(offset);
				tex.draw(spr);
			}
			offset.x += layerSize.x;
		}

		offset.x = 0;
		offset.y += layerSize.y;
		
	}
	
	sf::Image finalImage = tex.getTexture().copyToImage();
	finalImage.flipVertically();

	std::wstring filename = path.wstring();
	if (!finalImage.saveToFile(filename)) {
		DebugError(L"MainMenu::exportAsFile: failed to save file.");
		exit(0);
	}

	DebugLog(L"export " + filename);;
}

void MainMenu::importAnimation(std::vector<std::shared_ptr<Animation>> newAnimations) {

	animations = newAnimations;

	canvas->resize(sf::Vector2i(getAnimation(0)->getLayer(0)->_image.getSize()));
	layers_dialog->loadLayersFromCurrentFrame();
}

void MainMenu::cursorHover() {

	if (!dialogs.empty())
		return;

	if (selection->_state == SelectionState::Selecting)
		return;

	if (_rect.getGlobalBounds().contains(sf::Vector2f(cursor->_worldMousePosition))) {
  		ElementGUI_hovered = this->shared_from_this();
	}

	for (auto& mb : _menu_boxes)
		mb->cursorHover();

}

void MainMenu::handleEvent(const sf::Event& event) {

	if (!dialogs.empty())
		return;

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

	if (_state == MainMenuStates::Opened) {
		
		std::shared_ptr<MenuBox> hoverBox = std::dynamic_pointer_cast<MenuBox>(ElementGUI_hovered);
		if (hoverBox != nullptr && _open_menu_box != hoverBox) {
			hideMenu();
			openMenuBox(hoverBox);
		}
	}

	if (_state == MainMenuStates::Closing) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			if (!clicked_in_menu) {
				hideMenu();
				return;
			}
		}

		if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>(); mws && mws->delta != 0) {
			if (!clicked_in_menu) {
				hideMenu();
				return;
			}

		}
	}
	
	if (_state == MainMenuStates::Opened) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			if (!clicked_in_menu) {
				hideMenu();
			}
		}

		if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>(); mws && mws->delta != 0) {
			if (!clicked_in_menu) {
				hideMenu();
			}
		}
	}

	if(!clicked_in_menu)
		if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			_state = MainMenuStates::Closed;
		}

	if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>(); mws && mws->delta != 0) {
		_state = MainMenuStates::Closed;
	}
		
}

void MainMenu::update() {
	for (auto& mb : _menu_boxes)
		mb->update();

}

void MainMenu::draw() {
	window->draw(_rect);
	window->draw(*_logo);

	if (_open_menu_box != nullptr) {
		if (_open_menu_box->_options.size() > 0) {

			sf::Vector2f rectSize;
			rectSize.x = _open_menu_box->_options.front()->_rect.getSize().x;
			rectSize.y = (float)(_open_menu_box->_options.size()) * (float)(menu_height);

			sf::RectangleShape rect(rectSize);
			rect.setPosition(_open_menu_box->_options.front()->_rect.getPosition());
			rect.setOutlineThickness((float)(menuoptions_border_width));
			rect.setOutlineColor(menuoptions_border_color);
			window->draw(rect);
		}
	}

	for (auto& mb : _menu_boxes)
		mb->draw();
}

/////////////////////////////////////////////////////////////////////////

std::shared_ptr<MainMenu> main_menu;