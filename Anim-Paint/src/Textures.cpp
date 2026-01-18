#include "Textures.hpp"
#include "SFML/Graphics.hpp"

std::string ConvertWideToUtf8(std::wstring wide) {
	return std::string(wide.begin(), wide.end());
}

/////////////////////
// Texture class

Texture::Texture(std::wstring path) {
	_path = path;
	_texture = std::make_shared<sf::Texture>();
	_texture->loadFromFile(path);
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
	textures.push_back(std::make_shared<Texture>(path));
}

void loadTextures() {

	// logos
	loadTexture(L"tex\\logo\\small_logo.png");

	// cursor
	loadTexture(L"tex\\cursor\\brush.png");
	loadTexture(L"tex\\cursor\\fill.png");
	loadTexture(L"tex\\cursor\\picker.png");
	

	// file dialogs
	loadTexture(L"tex\\file_dialog\\harddrive.png");
	loadTexture(L"tex\\file_dialog\\dictionary.png");
	loadTexture(L"tex\\file_dialog\\file.png");
	loadTexture(L"tex\\file_dialog\\empty.png");
	loadTexture(L"tex\\file_dialog\\dictionaryIsClosed.png");
	loadTexture(L"tex\\file_dialog\\dictionaryIsOpened.png");
	loadTexture(L"tex\\file_dialog\\dictionaryIsEmpty.png");

	// slider
	loadTexture(L"tex\\slider\\bar.png");
	loadTexture(L"tex\\slider\\slider.png");

	// buttons 32x32
	loadTexture(L"tex\\btn32\\close.png");
	loadTexture(L"tex\\btn32\\close_hover.png");

	loadTexture(L"tex\\btn32\\first.png");
	loadTexture(L"tex\\btn32\\first_hover.png");

	loadTexture(L"tex\\btn32\\prev.png");
	loadTexture(L"tex\\btn32\\prev_hover.png");

	loadTexture(L"tex\\btn32\\next.png");
	loadTexture(L"tex\\btn32\\next_hover.png");

	loadTexture(L"tex\\btn32\\last.png");
	loadTexture(L"tex\\btn32\\last_hover.png");

	loadTexture(L"tex\\btn32\\add_frame.png");
	loadTexture(L"tex\\btn32\\add_frame_hover.png");

	loadTexture(L"tex\\btn32\\sub_frame.png");
	loadTexture(L"tex\\btn32\\sub_frame_hover.png");

	loadTexture(L"tex\\btn32\\move_next.png");
	loadTexture(L"tex\\btn32\\move_next_hover.png");

	loadTexture(L"tex\\btn32\\move_back.png");
	loadTexture(L"tex\\btn32\\move_back_hover.png");

	loadTexture(L"tex\\btn32\\visible.png");
	loadTexture(L"tex\\btn32\\visible_hover.png");

	loadTexture(L"tex\\btn32\\unvisible.png");
	loadTexture(L"tex\\btn32\\unvisible_hover.png");

	

	// tools
	loadTexture(L"tex\\tools\\bottom_arrow.png");
	loadTexture(L"tex\\tools\\bottom_arrow_hover.png");

	loadTexture(L"tex\\tools\\frame.png");
	loadTexture(L"tex\\tools\\frame_hover.png");

	loadTexture(L"tex\\tools\\frame_large.png");
	loadTexture(L"tex\\tools\\frame_large_hover.png");

	loadTexture(L"tex\\tools\\btn_none.png");
	loadTexture(L"tex\\tools\\btn_none_hover.png");

	loadTexture(L"tex\\tools\\btn_paste.png");
	loadTexture(L"tex\\tools\\btn_paste_hover.png");
	
	loadTexture(L"tex\\tools\\btn_cut.png");
	loadTexture(L"tex\\tools\\btn_cut_hover.png");

	loadTexture(L"tex\\tools\\btn_copy.png");
	loadTexture(L"tex\\tools\\btn_copy_hover.png");

	loadTexture(L"tex\\tools\\btn_select.png");
	loadTexture(L"tex\\tools\\btn_select_hover.png");

	loadTexture(L"tex\\tools\\btn_lasso.png");
	loadTexture(L"tex\\tools\\btn_lasso_hover.png");

	loadTexture(L"tex\\tools\\btn_brush.png");
	loadTexture(L"tex\\tools\\btn_brush_hover.png");

	loadTexture(L"tex\\tools\\btn_fill.png");
	loadTexture(L"tex\\tools\\btn_fill_hover.png");

	loadTexture(L"tex\\tools\\btn_picker.png");
	loadTexture(L"tex\\tools\\btn_picker_hover.png");

	loadTexture(L"tex\\tools\\btn_eraser.png");
	loadTexture(L"tex\\tools\\btn_eraser_hover.png");

	loadTexture(L"tex\\tools\\btn_size_decrease.png");
	loadTexture(L"tex\\tools\\btn_size_decrease_hover.png");

	loadTexture(L"tex\\tools\\btn_size_increase.png");
	loadTexture(L"tex\\tools\\btn_size_increase_hover.png");

	loadTexture(L"tex\\tools\\btn_palette_colors.png");
	loadTexture(L"tex\\tools\\btn_palette_colors_hover.png");
}
