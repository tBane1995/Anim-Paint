#pragma once
#include "SFML/Graphics.hpp"

void remove(sf::Image& image, sf::IntRect rect, sf::Color alphaColor = sf::Color::Transparent);
void remove(sf::Image& image, sf::IntRect rect, sf::Image* mask, sf::Color alphaColor = sf::Color::Transparent);

void copy(sf::Image* dst, sf::Image* src, sf::IntRect srcRect);
void copy(sf::Image* dst, sf::Image* src, sf::IntRect srcRect, sf::Color emptyColor = sf::Color::Transparent);

void paste(sf::Image* dst, sf::Image* src, int dstX, int dstY, sf::Color alphaColor = sf::Color::Transparent);
void paste(sf::Image* dst, sf::Image* src, int dstX, int dstY, sf::Image* mask, sf::Color alphaColor = sf::Color::Transparent);

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
	void draw(const sf::Vector2f& canvasPos, sf::Vector2i canvasSize, float scale);
};

extern Selection* selection;