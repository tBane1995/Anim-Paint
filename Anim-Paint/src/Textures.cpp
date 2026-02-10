#include "Textures.hpp"
#include "SFML/Graphics.hpp"
#include "DebugLog.hpp"

std::string ConvertWideToUtf8(std::wstring wide) {
	return std::string(wide.begin(), wide.end());
}

/////////////////////
// Texture class

Texture::Texture(std::wstring path, bool& loadingStatus) {
	_path = path;
	_texture = std::make_shared<sf::Texture>();
	if (!_texture->loadFromFile(path)) {
		loadingStatus = false;
	}
}

Texture::~Texture() {

}

////////////////////

std::vector<std::shared_ptr<Texture>> textures;


std::shared_ptr<Texture> getTexture(std::wstring path) {
	for (auto& t : textures) {
		if (t->_path == path)
			return t;
	}

	return nullptr;
}

void loadTexture(std::wstring path) {

	bool loadingStatus = true;
	std::shared_ptr<Texture> texture = std::make_shared<Texture>(path, loadingStatus);

	if(loadingStatus)
		textures.push_back(texture);
}

void loadTextures() {

	std::vector<std::wstring> texturePaths;

	// logos
	texturePaths.push_back(L"tex\\logo\\small_logo.png");

	// cursor
	texturePaths.push_back(L"tex\\cursor\\brush.png");
	texturePaths.push_back(L"tex\\cursor\\fill.png");
	texturePaths.push_back(L"tex\\cursor\\picker.png");
	
	// file dialogs
	texturePaths.push_back(L"tex\\file_dialog\\harddrive.png");
	texturePaths.push_back(L"tex\\file_dialog\\dictionary.png");
	texturePaths.push_back(L"tex\\file_dialog\\file.png");
	texturePaths.push_back(L"tex\\file_dialog\\empty.png");
	texturePaths.push_back(L"tex\\file_dialog\\dictionaryIsClosed.png");
	texturePaths.push_back(L"tex\\file_dialog\\dictionaryIsOpened.png");
	texturePaths.push_back(L"tex\\file_dialog\\dictionaryIsEmpty.png");

	// buttons 32x32
	texturePaths.push_back(L"tex\\btn32\\close.png");
	texturePaths.push_back(L"tex\\btn32\\close_hover.png");

	texturePaths.push_back(L"tex\\btn32\\first.png");
	texturePaths.push_back(L"tex\\btn32\\first_hover.png");

	texturePaths.push_back(L"tex\\btn32\\prev.png");
	texturePaths.push_back(L"tex\\btn32\\prev_hover.png");

	texturePaths.push_back(L"tex\\btn32\\next.png");
	texturePaths.push_back(L"tex\\btn32\\next_hover.png");

	texturePaths.push_back(L"tex\\btn32\\last.png");
	texturePaths.push_back(L"tex\\btn32\\last_hover.png");

	texturePaths.push_back(L"tex\\btn32\\add_frame.png");
	texturePaths.push_back(L"tex\\btn32\\add_frame_hover.png");

	texturePaths.push_back(L"tex\\btn32\\sub_frame.png");
	texturePaths.push_back(L"tex\\btn32\\sub_frame_hover.png");

	texturePaths.push_back(L"tex\\btn32\\move_next.png");
	texturePaths.push_back(L"tex\\btn32\\move_next_hover.png");

	texturePaths.push_back(L"tex\\btn32\\move_back.png");
	texturePaths.push_back(L"tex\\btn32\\move_back_hover.png");

	texturePaths.push_back(L"tex\\btn32\\visible.png");
	texturePaths.push_back(L"tex\\btn32\\visible_hover.png");

	texturePaths.push_back(L"tex\\btn32\\unvisible.png");
	texturePaths.push_back(L"tex\\btn32\\unvisible_hover.png");

	texturePaths.push_back(L"tex\\btn32\\unchecked.png");
	texturePaths.push_back(L"tex\\btn32\\unchecked_hover.png");
								  
	texturePaths.push_back(L"tex\\btn32\\checked.png");
	texturePaths.push_back(L"tex\\btn32\\checked_hover.png");

	// tools
	texturePaths.push_back(L"tex\\tools\\bottom_arrow.png");
	texturePaths.push_back(L"tex\\tools\\bottom_arrow_hover.png");

	texturePaths.push_back(L"tex\\tools\\frame.png");
	texturePaths.push_back(L"tex\\tools\\frame_hover.png");

	texturePaths.push_back(L"tex\\tools\\frame_large.png");
	texturePaths.push_back(L"tex\\tools\\frame_large_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_none.png");
	texturePaths.push_back(L"tex\\tools\\btn_none_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_paste.png");
	texturePaths.push_back(L"tex\\tools\\btn_paste_hover.png");
	
	texturePaths.push_back(L"tex\\tools\\btn_cut.png");
	texturePaths.push_back(L"tex\\tools\\btn_cut_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_copy.png");
	texturePaths.push_back(L"tex\\tools\\btn_copy_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_select.png");
	texturePaths.push_back(L"tex\\tools\\btn_select_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_lasso.png");
	texturePaths.push_back(L"tex\\tools\\btn_lasso_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_brush.png");
	texturePaths.push_back(L"tex\\tools\\btn_brush_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_fill.png");
	texturePaths.push_back(L"tex\\tools\\btn_fill_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_picker.png");
	texturePaths.push_back(L"tex\\tools\\btn_picker_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_eraser.png");
	texturePaths.push_back(L"tex\\tools\\btn_eraser_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_size_decrease.png");
	texturePaths.push_back(L"tex\\tools\\btn_size_decrease_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_size_increase.png");
	texturePaths.push_back(L"tex\\tools\\btn_size_increase_hover.png");

	texturePaths.push_back(L"tex\\tools\\btn_palette_colors.png");
	texturePaths.push_back(L"tex\\tools\\btn_palette_colors_hover.png");

	

	// bottom bar
	texturePaths.push_back(L"tex\\bottom_bar\\cursor_position.png");
	texturePaths.push_back(L"tex\\bottom_bar\\canvas_size.png");
	texturePaths.push_back(L"tex\\bottom_bar\\selection_size.png");

	// load all textures
	for (auto& path : texturePaths) {
		loadTexture(path);
	}

	// Loaded textures
	DebugLog(L"Loading textures:");
	for (auto& path : texturePaths) {
		if (getTexture(path)) {
			DebugLog(path);
		}
	}

	// Failed textures
	bool failed = false;
	for (auto& path : texturePaths) {
		if (!getTexture(path)) {
			if (!failed) {
				DebugError(L"Failed to load textures:");
			}
			DebugError(path);
			failed = true;
		}
	}

	if (failed) {
		exit(0);
	}

}
