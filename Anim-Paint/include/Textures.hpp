#pragma once
#include "SFML/Graphics.hpp"

std::string ConvertWideToUtf8(std::wstring wide);

class Texture {
public:
	std::wstring _path;
	std::shared_ptr<sf::Texture> _texture;

	Texture(std::wstring path, bool& loadingStatus);
	~Texture();
};

extern std::vector<std::shared_ptr<Texture>> textures;

std::shared_ptr<Texture> getTexture(std::wstring path);
void loadTexture(std::wstring path);
void loadTextures();