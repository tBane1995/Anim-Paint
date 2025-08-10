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
	// dialog - frames
	loadTexture(L"tex\\frames\\first.png");
	loadTexture(L"tex\\frames\\prev.png");
	loadTexture(L"tex\\frames\\next.png");
	loadTexture(L"tex\\frames\\last.png");

	loadTexture(L"tex\\frames\\first_hover.png");
	loadTexture(L"tex\\frames\\prev_hover.png");
	loadTexture(L"tex\\frames\\next_hover.png");
	loadTexture(L"tex\\frames\\last_hover.png");

	// dialog - layers
	loadTexture(L"tex\\layers\\visible.png");
	loadTexture(L"tex\\layers\\unvisible.png");

	loadTexture(L"tex\\layers\\visible_hover.png");
	loadTexture(L"tex\\layers\\unvisible_hover.png");

	// dialog - colors
	loadTexture(L"tex\\colors\\frame.png");
	loadTexture(L"tex\\colors\\frame_hover.png");
}

#endif