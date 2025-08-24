#ifndef Textures_hpp
#define Textures_hpp

// TO-DO
std::string ConvertWideToUtf8(std::wstring wide) {
	return std::string(wide.begin(), wide.end());
}

class Texture {
public:
	std::wstring path;
	sf::Texture* texture;

	Texture(std::wstring path) {
		this->path = path;
		texture = new sf::Texture();
		texture->loadFromFile(ConvertWideToUtf8(path));
	}
};

std::vector < Texture* > textures;

Texture* getTexture(std::wstring path) {
	for (auto& t : textures) {
		if (t->path == path)
			return t;
	}

	return nullptr;
}

void loadTexture(std::wstring path) {
	textures.push_back(new Texture(path));
}

void loadTextures() {

	// logos
	loadTexture(L"tex\\logo\\small_logo.png");

	// dialogs
	loadTexture(L"tex\\dialog\\close.png");
	loadTexture(L"tex\\dialog\\close_hover.png");

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

#endif