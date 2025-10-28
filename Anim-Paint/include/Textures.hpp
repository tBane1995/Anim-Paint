#pragma once
#include "SFML/Graphics.hpp"

std::string ConvertWideToUtf8(std::wstring wide);

class Texture {
public:
	std::wstring _path;
	sf::Texture* _texture;

	Texture(std::wstring path);
};

extern sf::Texture emptyTexture;
extern sf::Texture emptyTexture_128x128;
extern std::vector < Texture* > textures;

Texture* getTexture(std::wstring path);
void loadTexture(std::wstring path);
void loadTextures();