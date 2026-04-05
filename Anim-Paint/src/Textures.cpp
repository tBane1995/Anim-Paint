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
	texturePaths.push_back(L"tex\\cursor\\cross.png");
	
	// main icons
	texturePaths.push_back(L"tex\\unchecked.png");
	texturePaths.push_back(L"tex\\unchecked_hover.png");

	texturePaths.push_back(L"tex\\checked.png");
	texturePaths.push_back(L"tex\\checked_hover.png");

	// main menu icons
	texturePaths.push_back(L"tex\\main_menu\\new_project.png");
	texturePaths.push_back(L"tex\\main_menu\\open_project.png");
	texturePaths.push_back(L"tex\\main_menu\\save_project.png");
	texturePaths.push_back(L"tex\\main_menu\\export.png");
	texturePaths.push_back(L"tex\\main_menu\\import.png");
	texturePaths.push_back(L"tex\\main_menu\\undo.png");
	texturePaths.push_back(L"tex\\main_menu\\redo.png");

	// dialogs
	texturePaths.push_back(L"tex\\dialog\\increase.png");
	texturePaths.push_back(L"tex\\dialog\\increase_hover.png");
	texturePaths.push_back(L"tex\\dialog\\decrease.png");
	texturePaths.push_back(L"tex\\dialog\\decrease_hover.png");

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
	texturePaths.push_back(L"tex\\btn32\\close_press.png");

	texturePaths.push_back(L"tex\\btn32\\first.png");
	texturePaths.push_back(L"tex\\btn32\\first_hover.png");
	texturePaths.push_back(L"tex\\btn32\\first_press.png");


	texturePaths.push_back(L"tex\\btn32\\prev.png");
	texturePaths.push_back(L"tex\\btn32\\prev_hover.png");
	texturePaths.push_back(L"tex\\btn32\\prev_press.png");

	texturePaths.push_back(L"tex\\btn32\\next.png");
	texturePaths.push_back(L"tex\\btn32\\next_hover.png");
	texturePaths.push_back(L"tex\\btn32\\next_press.png");

	texturePaths.push_back(L"tex\\btn32\\top.png");
	texturePaths.push_back(L"tex\\btn32\\top_hover.png");
	texturePaths.push_back(L"tex\\btn32\\top_press.png");

	texturePaths.push_back(L"tex\\btn32\\bottom.png");
	texturePaths.push_back(L"tex\\btn32\\bottom_hover.png");
	texturePaths.push_back(L"tex\\btn32\\bottom_press.png");

	texturePaths.push_back(L"tex\\btn32\\last.png");
	texturePaths.push_back(L"tex\\btn32\\last_hover.png");
	texturePaths.push_back(L"tex\\btn32\\last_press.png");

	texturePaths.push_back(L"tex\\btn32\\add.png");
	texturePaths.push_back(L"tex\\btn32\\add_hover.png");
	texturePaths.push_back(L"tex\\btn32\\add_press.png");

	texturePaths.push_back(L"tex\\btn32\\remove.png");
	texturePaths.push_back(L"tex\\btn32\\remove_hover.png");
	texturePaths.push_back(L"tex\\btn32\\remove_press.png");

	texturePaths.push_back(L"tex\\btn32\\move_next.png");
	texturePaths.push_back(L"tex\\btn32\\move_next_hover.png");
	texturePaths.push_back(L"tex\\btn32\\move_next_press.png");

	texturePaths.push_back(L"tex\\btn32\\move_back.png");
	texturePaths.push_back(L"tex\\btn32\\move_back_hover.png");
	texturePaths.push_back(L"tex\\btn32\\move_back_press.png");

	texturePaths.push_back(L"tex\\btn32\\visible.png");
	texturePaths.push_back(L"tex\\btn32\\visible_hover.png");
	texturePaths.push_back(L"tex\\btn32\\visible_press.png");

	texturePaths.push_back(L"tex\\btn32\\unvisible.png");
	texturePaths.push_back(L"tex\\btn32\\unvisible_hover.png");
	texturePaths.push_back(L"tex\\btn32\\unvisible_press.png");

	texturePaths.push_back(L"tex\\btn32\\unchecked.png");
	texturePaths.push_back(L"tex\\btn32\\unchecked_hover.png");
	texturePaths.push_back(L"tex\\btn32\\unchecked_press.png");
								  
	texturePaths.push_back(L"tex\\btn32\\checked.png");
	texturePaths.push_back(L"tex\\btn32\\checked_hover.png");
	texturePaths.push_back(L"tex\\btn32\\checked_press.png");

	// tools
	texturePaths.push_back(L"tex\\tools\\bottom_arrow.png");
	texturePaths.push_back(L"tex\\tools\\bottom_arrow_hover.png");

	texturePaths.push_back(L"tex\\tools\\frame.png");
	texturePaths.push_back(L"tex\\tools\\frame_hover.png");

	texturePaths.push_back(L"tex\\tools\\frame_large.png");
	texturePaths.push_back(L"tex\\tools\\frame_large_hover.png");

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

	texturePaths.push_back(L"tex\\tools\\resizable_tools\\circle.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\circle_hover.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\circle_press.png");

	texturePaths.push_back(L"tex\\tools\\resizable_tools\\triangle.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\triangle_hover.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\triangle_press.png");

	texturePaths.push_back(L"tex\\tools\\resizable_tools\\rectangle.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\rectangle_hover.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\rectangle_press.png");

	texturePaths.push_back(L"tex\\tools\\resizable_tools\\diamond.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\diamond_hover.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\diamond_press.png");

	texturePaths.push_back(L"tex\\tools\\resizable_tools\\pentagon.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\pentagon_hover.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\pentagon_press.png");

	texturePaths.push_back(L"tex\\tools\\resizable_tools\\hexagon_flat_top.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\hexagon_flat_top_hover.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\hexagon_flat_top_press.png");

	texturePaths.push_back(L"tex\\tools\\resizable_tools\\hexagon_point_top.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\hexagon_point_top_hover.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\hexagon_point_top_press.png");

	texturePaths.push_back(L"tex\\tools\\resizable_tools\\octagon.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\octagon_hover.png");
	texturePaths.push_back(L"tex\\tools\\resizable_tools\\octagon_press.png");

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
			DebugStat(path);
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
