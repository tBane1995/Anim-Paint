#pragma once
#include "SFML/Graphics.hpp"

enum class LassoState { None, Selecting, Selected, Moving };

class Lasso {
public:
	LassoState _state;

	std::vector<sf::Vector2i> _points;
	
	// offset
	sf::Vector2i _offset;	// to move

	// image
	sf::Image* _image;
	sf::Texture _texture;
	sf::Sprite _sprite;

	// outline
	sf::RenderTexture _outlineRenderTexture;
	sf::Vector2i _outlineOffset;
	sf::Sprite _outlineSprite;
	
	// outside rect
	sf::IntRect _rect;

	// shader
	sf::Shader _shader;

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
