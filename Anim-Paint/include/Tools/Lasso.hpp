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
	std::shared_ptr<sf::Image> _image;
	sf::Texture _texture;
	std::shared_ptr<sf::Sprite> _sprite;

	// outline
	sf::RenderTexture _outlineRenderTexture;
	sf::Vector2i _outlineOffset;
	std::shared_ptr<sf::Sprite> _outlineSprite;
	
	// outside rect
	sf::IntRect _rect;

	// mask
	sf::Image _maskImage;

	// shader
	sf::Shader _shader;

	Lasso();
	~Lasso();

	void shiftOriginIfNeeded(sf::Vector2i& point);
	void addPoint(sf::Vector2i point);
	void unselect();
	void selectAll();
	void generateRect();
	bool clickOnSelection(sf::Vector2i point);
	void copy(sf::Image& canvas, sf::Color emptyColor);
	void paste(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor);
	bool paste(sf::Image& canvas, sf::Color emptyColor);
	void cut(sf::Image& canvas, sf::Color emptyColor);
	void generateOutline(bool selectionComplete = false);
	bool pointOnSegment(sf::Vector2i p, sf::Vector2i a, sf::Vector2i b);
	bool isPointInPolygon(sf::Vector2i p, std::vector<sf::Vector2i>& poly);
	void generateMask();
	void drawImage(sf::Vector2i canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor, bool useMask = false);
	void drawOutline(sf::Vector2i canvasPosition, float scale);
	void drawRect(sf::Vector2i canvasPosition, float scale);
	void draw(sf::Vector2i canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor);
};

extern std::shared_ptr<Lasso> lasso;
