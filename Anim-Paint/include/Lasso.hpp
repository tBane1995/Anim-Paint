#pragma once
#include "SFML/Graphics.hpp"

enum class LassoState { None, Selecting, Selected, Moving };

class Lasso {
public:
	LassoState state;

	std::vector<sf::Vector2i> points;
	
	// offset
	sf::Vector2i offset;	// to move

	// image
	sf::Image* image;
	sf::Texture texture;
	sf::Sprite sprite;

	// outline
	sf::RenderTexture outlineRenderTexture;
	sf::Vector2i outlineOffset;
	sf::Sprite outlineSprite;
	
	// outside rect
	sf::IntRect rect;

	// shader
	sf::Shader shader;

	Lasso();
	~Lasso();

	void shiftOriginIfNeeded(sf::Vector2i& point);
	void addPoint(sf::Vector2i point);
	void unselect();
	void generateRect();
	bool clickOnSelection(sf::Vector2i point);
	void copy(sf::Image* canvas, sf::Color emptyColor);
	void paste(sf::Image* canvas, sf::Color emptyColor);
	void cut(sf::Image* canvas, sf::Color emptyColor);
	void generateOutline(bool selectionComplete = false);
	sf::RenderTexture* generateMask();
	void drawImage(sf::Vector2f canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor, bool useMask = false);
	void drawOutline(sf::Vector2f canvasPosition, float scale);
	void drawRect(sf::Vector2f canvasPosition, float scale);
	void draw(sf::Vector2f canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor);
};

extern Lasso* lasso;
