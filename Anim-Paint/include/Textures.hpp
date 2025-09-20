#pragma once
#include "SFML/Graphics.hpp"

std::string ConvertWideToUtf8(std::wstring wide);

class Texture {
public:
	std::wstring path;
	sf::Texture* texture;

	Texture(std::wstring path);
};

extern std::vector < Texture* > textures;

Texture* getTexture(std::wstring path);
void loadTexture(std::wstring path);
void loadTextures();