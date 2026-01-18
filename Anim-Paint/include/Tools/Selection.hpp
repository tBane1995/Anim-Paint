#pragma once
#include "SFML/Graphics.hpp"

void removeImage(sf::Image& image, sf::IntRect rect, sf::Color alphaColor = sf::Color::Transparent);
void removeImage(sf::Image& image, sf::IntRect rect, sf::Image& mask, sf::Color alphaColor = sf::Color::Transparent);

void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect);
void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect, sf::Color emptyColor = sf::Color::Transparent);
void copyImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, int srcX, int srcY, sf::Image& mask, sf::Color alphaColor);

void pasteImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Color alphaColor = sf::Color::Transparent);
void pasteImage(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor = sf::Color::Transparent);

enum class SelectionState { None, Selecting, Selected, Moving };

class Selection {
public:	
	SelectionState _state;
	sf::IntRect _rect;
	sf::Vector2i _offset;
	sf::Image* _img;

	Selection();
	~Selection();

	sf::IntRect normalizeRect();
	void paste(sf::Image* editorImage, sf::Image*& bufferSelection);
	void copy(sf::Image* editorImage, sf::Image* bufferSelection);
	void cut(sf::Image* editorImage, sf::Image* bufferSelection, sf::Color emptyColor);
	bool clickOnSelection(sf::Vector2i point);
	void draw(const sf::Vector2i& canvasPos, sf::Vector2i canvasSize, float scale);
};

extern Selection* selection;