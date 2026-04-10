#include "Components/MainMenu/MainMenu.hpp"
#include "Theme.hpp"
#include "Time.hpp"
#include "Cursor.hpp"
#include "Window.hpp"

#include "Dialogs/Dialog.hpp"
#include "Dialogs/FileDialog/FileDialog.hpp"
#include "Dialogs/Dialog_Save_Project.hpp"
#include "Dialogs/Dialog_Open_Project.hpp"
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

#include "Components/AnimationsPanel.hpp"
#include "Components/FramesPanel.hpp"
#include "Components/LayersPanel/LayersPanel.hpp"
#include "Components/PreviewAnimationPanel.hpp"
#include "Animation/Animation.hpp"
#include <iostream>
#include <filesystem>
#include "Tools/Selection.hpp"
#include "Components/Canvas.hpp"
#include "History.hpp"
#include <fstream>
#include "DebugLog.hpp"
#include "Dialogs/ConfirmDialog.hpp"
#include <algorithm>


MainMenu::MainMenu() : Element() {

	// FILE
	file = std::make_shared<MenuButton>(L"File");
	file->_onclick_func = [this]() {
		hideMenu();
		openMenuButton(file);
		};
	_menu_boxes.push_back(file);

	file_new = std::make_shared<OptionWithIcon>(
		L"New file", 
		getTexture(L"tex\\main_menu\\new_project.png"), 
		getTexture(L"tex\\main_menu\\new_project.png"), 
		L"Ctrl+N");

	file_new->_onclick_func = [this]() {
		std::shared_ptr<ConfirmDialog> confirmDialog = std::make_shared<ConfirmDialog>(L"New file", L"Are you sure you want to create\na new file?\nUnsaved progress will be lost.", sf::Vector2i(200, 200));
		confirmDialog->_confirmBtn->_onclick_func = [this, confirmDialog]() {

			std::shared_ptr<Animation> animation = std::make_shared<Animation>();
			animation->addEmptyFrame(sf::Vector2i(32, 32));
			
			animations.clear();
			animations.push_back(animation);
			
			history->clear();
			history->saveStep();

			currentAnimationId = 0;
			getCurrentAnimation()->firstFrame();
			getCurrentAnimation()->firstLayer();

			animations_panel->updateText();
			frames_panel->updateText();
			layers_panel->loadLayersFromCurrentFrame();
			canvas->reset();
			canvas->setCenter();
			confirmDialog->_state = DialogState::ToClose;
			};
		dialogs.push_back(confirmDialog);
		closeMenu();
		};

	// TO-DO
	//std::shared_ptr<Option> file_save = std::make_shared<Option>(L"Save", L"Ctrl+S");

	file_saveAs = std::make_shared<OptionWithIcon>(
		L"Save as", 
		getTexture(L"tex\\main_menu\\save_project.png"),
		getTexture(L"tex\\main_menu\\save_project.png"),
		L"Ctrl+S");

	file_saveAs->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Save_Project>());
		closeMenu();
		};

	file_load = std::make_shared<OptionWithIcon>(
		L"Open", 
		getTexture(L"tex\\main_menu\\open_project.png"),
		getTexture(L"tex\\main_menu\\open_project.png"),
		L"Ctrl+O");

	file_load->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Open_Project>());
		closeMenu();
		};

	file_export = std::make_shared<OptionWithIcon>(
		L"Export", 
		getTexture(L"tex\\main_menu\\export.png"),
		getTexture(L"tex\\main_menu\\export.png"),
		L"Ctrl+E");

	file_export->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Export>());
		closeMenu();
		};

	file_import = std::make_shared<OptionWithIcon>(
		L"Import", 
		getTexture(L"tex\\main_menu\\import.png"),
		getTexture(L"tex\\main_menu\\import.png"),
		L"Ctrl+I");

	file_import->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Import>());
		closeMenu();
		};

	file->addOption(file_new);
	file->addOption(file_saveAs);
	file->addOption(file_load);
	file->addOption(file_export);
	file->addOption(file_import);

	// EDIT
	edit = std::make_shared<MenuButton>(L"Edit");
	edit->_onclick_func = [this]() {
		hideMenu();
		openMenuButton(edit);
		};
	_menu_boxes.push_back(edit);

	edit_undo = std::make_shared<OptionWithIcon>(
		L"Undo", 
		getTexture(L"tex\\main_menu\\undo.png"),
		getTexture(L"tex\\main_menu\\undo.png"),
		L"Ctrl+Z");

	edit_undo->_onclick_func = [this]() {
		history->undo();
		};
	edit_undo->setActive(false);

	edit_redo = std::make_shared<OptionWithIcon>(
		L"Redo",
		getTexture(L"tex\\main_menu\\redo.png"),
		getTexture(L"tex\\main_menu\\redo.png"),
		L"Ctrl+Y");

	edit_redo->_onclick_func = [this]() {
		history->redo();
		};

	edit_redo->setActive(false);

	edit->addOption(edit_undo);
	edit->addOption(edit_redo);

	// TOOLS
	tools = std::make_shared<MenuButton>(L"Tools");
	tools->_onclick_func = [this]() {
		hideMenu();
		openMenuButton(tools);
		};
	_menu_boxes.push_back(tools);

	tools_resize = std::make_shared<Option>(L"Resize");
	tools_resize->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Resize>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools_rotation = std::make_shared<Option>(L"Rotation");
	tools_rotation->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Rotation>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools_brightness_contrast = std::make_shared<Option>(L"Brightness-Contrast");
	tools_brightness_contrast->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Brightness_Contrast>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools_hue_saturation = std::make_shared<Option>(L"Hue-Saturation");
	tools_hue_saturation->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Hue_Saturation>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools_sepia = std::make_shared<Option>(L"Sepia");
	tools_sepia->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Sepia>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools_outline = std::make_shared<Option>(L"Outline");
	tools_outline->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Outline>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools_invert = std::make_shared<Option>(L"Invert colors");
	tools_invert->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Invert_Colors>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools_chessboard = std::make_shared<Option>(L"Chessboard");
	tools_chessboard->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Chessboard>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools_smoothing = std::make_shared<Option>(L"Smoothing");
	tools_smoothing->_onclick_func = [this]() {
		dialogs.push_back(std::make_shared<Dialog_Smoothing>(getCurrentAnimation()->getLayers()));
		closeMenu();
		};

	tools->addOption(tools_resize);
	tools->addOption(tools_rotation);
	tools->addOption(tools_brightness_contrast);
	tools->addOption(tools_hue_saturation);
	tools->addOption(tools_sepia);
	tools->addOption(tools_outline);
	tools->addOption(tools_invert);
	tools->addOption(tools_chessboard);
	tools->addOption(tools_smoothing);

	// SELECT
	select = std::make_shared<MenuButton>(L"Select");
	select->_onclick_func = [this]() {
		hideMenu();
		openMenuButton(select);
		};
	_menu_boxes.push_back(select);

	select_all = std::make_shared<Option>(L"Select all", L"Ctrl+A");
	select_all->_onclick_func = [this]() {
		toolbar->selectToolButton(toolbar->_btn_select);
		toolbar->_toolType = ToolType::Selector;
		selection->selectAll();
		resizable_tool = selection;
		closeMenu();
		};

	select_none = std::make_shared<Option>(L"Select none", L"Ctrl+D");
	select_none->_onclick_func = [this]() {
		if (selection->_state != ResizableToolState::None) {
			selection->reset();
		}
		closeMenu();
		};

	select_align_center = std::make_shared<Option>(L"Align center");
	select_align_center->_onclick_func = [this]() {
		sf::Vector2i oldPos = selection->_rect.position;
		selection->_rect.position = (canvas->_size - selection->_resizedRect.size) / 2;
		selection->_resizedRect.position = selection->_rect.position;
		selection->_outlineOffset = selection->_rect.position - oldPos + selection->_outlineOffset;
		selection->generateEdgePoints();
		closeMenu();
		};

	select->addOption(select_all);
	select->addOption(select_none);
	select->addOption(select_align_center);

	// WINDOWS
	windows = std::make_shared<MenuButton>(L"Windows");
	windows->_onclick_func = [this]() {
		hideMenu();
		openMenuButton(windows);
		};
	_menu_boxes.push_back(windows);

	canvas_repeating = std::make_shared<OptionWithCheckbox>(L"Canvas repeating", getTexture(L"tex\\unchecked.png"), getTexture(L"tex\\unchecked_hover.png"));
	canvas_repeating->addValue(getTexture(L"tex\\checked.png"), getTexture(L"tex\\checked_hover.png"));

	window_animations = std::make_shared<Option>(L"Animations");
	window_animations->_onclick_func = [this]() {
		if (!isOpenStaticDialog(animations_panel)) {		
			animations_panel->_state = DialogState::Idle;
			static_dialogs.push_back(animations_panel);
			window_animations->setActive(false);
		}
		closeMenu();
		};

	window_frames = std::make_shared<Option>(L"Frames");
	window_frames->_onclick_func = [this]() {
		if (!isOpenStaticDialog(frames_panel)) {
			frames_panel->_state = DialogState::Idle;
			static_dialogs.push_back(frames_panel);
			window_frames->setActive(false);
		}
		closeMenu();
		};

	window_layers = std::make_shared<Option>(L"Layers");
	window_layers->_onclick_func = [this]() {
		std::shared_ptr<Dialog> ptr = std::dynamic_pointer_cast<Dialog>(layers_panel);
		if (!isOpenStaticDialog(layers_panel)) {
			layers_panel->_state = DialogState::Idle;
			static_dialogs.push_back(layers_panel);
			window_layers->setActive(false);
		}
		closeMenu();
		};

	window_preview_animation = std::make_shared<Option>(L"Preview Animation");
	window_preview_animation->_onclick_func = [this]() {
		if (!isOpenStaticDialog(preview_animation_panel)) {
			preview_animation_panel->_state = DialogState::Idle;
			static_dialogs.push_back(preview_animation_panel);
			window_preview_animation->setActive(false);
		}
		closeMenu();
		};
	
	setActiveForWindows();

	windows->addOption(canvas_repeating);
	windows->addOption(window_animations);
	windows->addOption(window_frames);
	windows->addOption(window_layers);
	windows->addOption(window_preview_animation);

	// POSITIONING
	_state = MainMenuStates::Closed;
	_open_menu_button = nullptr;

	resize();
	setPosition(sf::Vector2i(0, 0));
}

MainMenu::~MainMenu() {

}

void MainMenu::resize() {
	sf::Vector2i rectSize;
	rectSize.x = (int)(mainView.getSize().x);
	rectSize.y = menu_height;

	_rect.size = rectSize;
	_rect.position = sf::Vector2i(0, 0);
}

sf::Vector2i MainMenu::getSize() {
	return _rect.size;
}

void MainMenu::setPosition(sf::Vector2i position) {

	int x = 24;
	int y = position.y + menu_padding;
	for (int i = 0; i < _menu_boxes.size(); i++) {
		_menu_boxes[i]->setPosition(sf::Vector2i(position.x + x, y));
		x = x + (int)(_menu_boxes[i]->getSize().x);
	}
}

void MainMenu::setActiveForWindows() {

	window_animations->setActive(!isOpenStaticDialog(animations_panel));
	window_frames->setActive(!isOpenStaticDialog(frames_panel));
	window_layers->setActive(!isOpenStaticDialog(layers_panel));
	window_preview_animation->setActive(!isOpenStaticDialog(preview_animation_panel));

}

void MainMenu::hideMenu() {
	if (_open_menu_button != nullptr)
		_open_menu_button->_isSelected = false;

	_open_menu_button = nullptr;
	_state = MainMenuStates::Closing;
}

void MainMenu::closeMenu() {
	if (_open_menu_button != nullptr)
		_open_menu_button->_isSelected = false;

	_state = MainMenuStates::Closed;
	_open_menu_button = nullptr;
}


void MainMenu::openMenuButton(std::shared_ptr<MenuButton> menuButton) {
	_state = MainMenuStates::Opened;
	_open_menu_button = menuButton;
	_open_menu_button->_isSelected = true;
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
	
	//std::wcout << L"save " << path.wstring() << "\n";
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
	layers_panel->loadLayersFromCurrentFrame();
}

bool MainMenu::cursorOnAnyMenuButton() {
	for(auto& mb : _menu_boxes)
		if (mb->_rect.contains(cursor->_position))
			return true;

	return false;
}

void MainMenu::cursorHover() {

	if (!dialogs.empty())
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	if (canvas->_state != CanvasState::Idle)
		return;

	if (resizable_tool != nullptr && (resizable_tool->_state == ResizableToolState::Moving || resizable_tool->_state == ResizableToolState::Selecting || resizable_tool->_state == ResizableToolState::Resizing))
		return;

	if(!static_dialogs.empty() && static_dialogs.front()->_is_moved)
		return;

	if (_rect.contains(cursor->_position)) {
  		Element_hovered = this->shared_from_this();
	}

	for (auto& mb : _menu_boxes)
		mb->cursorHover();

}

void MainMenu::handleEvent(const sf::Event& event) {

	if (!dialogs.empty())
		return;

	if (!static_dialogs.empty() && static_dialogs.front()->_is_moved)
		return;

	if (const auto* kp = event.getIf<sf::Event::KeyPressed>(); kp) {
		if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::LControl)) {
			if(kp->code == sf::Keyboard::Key::N) {
				file_new->_onclick_func();
				closeMenu();
				return;
			}
			else if(kp->code == sf::Keyboard::Key::S) {
				file_saveAs->_onclick_func();
				closeMenu();
				return;
			}
			else if(kp->code == sf::Keyboard::Key::O) {
				file_load->_onclick_func();
				closeMenu();
				return;
			}
			else if(kp->code == sf::Keyboard::Key::E) {
				file_export->_onclick_func();
				closeMenu();
				return;
			}
			else if(kp->code == sf::Keyboard::Key::I) {
				file_import->_onclick_func();
				closeMenu();
				return;
			}
		}
	}

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	bool clicked_in_menu = false;

	for (auto& mb : _menu_boxes) {
		mb->handleEvent(event);
		if (Element_pressed == mb) {
			clicked_in_menu = true;
		}

		if (mb->_isSelected) {
			for (auto& op : mb->_options) {
				op->handleEvent(event);
				if (op->_rect.contains(cursor->_position)) {
					clicked_in_menu = true;
				}
			}
		}
	}

	if (_state == MainMenuStates::Opened) {
		
		std::shared_ptr<MenuButton> hoverBox = std::dynamic_pointer_cast<MenuButton>(Element_hovered);
		if (hoverBox != nullptr && _open_menu_button != hoverBox) {
			hideMenu();
			openMenuButton(hoverBox);
		}
	}

	if (_state == MainMenuStates::Closing) {
		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
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
	}

	if(!clicked_in_menu)
		if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			_state = MainMenuStates::Closed;
		}
		
}

void MainMenu::update() {
	for (auto& mb : _menu_boxes)
		mb->update();

	edit_redo->setActive(history->canRedo());
	edit_undo->setActive(history->canUndo());
}

void MainMenu::draw() {

	if (_open_menu_button && _open_menu_button->_isSelected) {
		
		sf::Vector2f rectSize;
		rectSize.x = (float)(_open_menu_button->_options.front()->getSize().x);
		rectSize.y = (float)(_open_menu_button->_options.size()) * (float)(optionbox_height);

		sf::RectangleShape rect(rectSize);
		rect.setPosition((sf::Vector2f)(_open_menu_button->_options.front()->getPosition()));
		rect.setOutlineThickness((float)(optionbox_border_width));
		rect.setOutlineColor(optionbox_border_color);
		window->draw(rect);

		for (auto& option : _open_menu_button->_options)
			option->draw();
	}


	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setFillColor(menu_bar_color);
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);
	
	sf::Sprite logo(*getTexture(L"tex\\logo\\small_logo.png")->_texture);
	logo.setPosition(sf::Vector2f(0, ((float)(menu_height)-logo.getGlobalBounds().size.y) / 2.0f));
	window->draw(logo);


	for (auto& mb : _menu_boxes)
		mb->draw();
}

/////////////////////////////////////////////////////////////////////////

std::shared_ptr<MainMenu> main_menu;