#include "Textures.hpp"
#include "SFML/Graphics.hpp"

std::string ConvertWideToUtf8(std::wstring wide) {
	return std::string(wide.begin(), wide.end());
}

/////////////////////
// Texture class

Texture::Texture(std::wstring path) {
	_path = path;
	_texture = new sf::Texture();
	_texture->loadFromFile(ConvertWideToUtf8(path));
}

////////////////////

std::vector < Texture* > textures;
sf::Texture emptyTexture;
sf::Texture emptyTexture_128x128;

Texture* getTexture(std::wstring path) {
	for (auto& t : textures) {
		if (t->_path == path)
			return t;
	}

	return nullptr;
}

void loadTexture(std::wstring path) {
	textures.push_back(new Texture(path));
}

void loadTextures() {

	{
		emptyTexture.resize(sf::Vector2u(1, 1));
		sf::Image img(sf::Vector2u(1, 1), sf::Color::Transparent);
		emptyTexture.update(img);
	}

	{
		emptyTexture_128x128.resize(sf::Vector2u(128, 128));
		sf::Image img(sf::Vector2u(128, 128), sf::Color::Transparent);
		emptyTexture_128x128.update(img);
	}

	// logos
	loadTexture(L"tex\\logo\\small_logo.png");

	// dialogs
	loadTexture(L"tex\\dialog\\close.png");
	loadTexture(L"tex\\dialog\\close_hover.png");
	loadTexture(L"tex\\dialog\\harddrive.png");
	loadTexture(L"tex\\dialog\\dictionary.png");
	loadTexture(L"tex\\dialog\\file.png");
	loadTexture(L"tex\\dialog\\empty.png");
	loadTexture(L"tex\\dialog\\dictionaryIsClosed.png");
	loadTexture(L"tex\\dialog\\dictionaryIsOpened.png");
	loadTexture(L"tex\\dialog\\dictionaryIsEmpty.png");

	// slider
	loadTexture(L"tex\\slider\\bar.png");
	loadTexture(L"tex\\slider\\slider.png");

	// dialog - frames
	loadTexture(L"tex\\frames\\first.png");
	loadTexture(L"tex\\frames\\first_hover.png");

	loadTexture(L"tex\\frames\\prev.png");
	loadTexture(L"tex\\frames\\prev_hover.png");

	loadTexture(L"tex\\frames\\next.png");
	loadTexture(L"tex\\frames\\next_hover.png");

	loadTexture(L"tex\\frames\\last.png");
	loadTexture(L"tex\\frames\\last_hover.png");


	// dialog - layers
	loadTexture(L"tex\\layers\\visible.png");
	loadTexture(L"tex\\layers\\visible_hover.png");

	loadTexture(L"tex\\layers\\unvisible.png");
	loadTexture(L"tex\\layers\\unvisible_hover.png");

	

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
}
