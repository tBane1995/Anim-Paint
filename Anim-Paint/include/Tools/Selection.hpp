#pragma once
#include "SFML/Graphics.hpp"
#include "ElementGUI/EdgePoint.hpp"

void removeImageWithAlpha(sf::Image& image, sf::IntRect rect, sf::Color alphaColor = sf::Color::Transparent);
void removeImageWithMask(sf::Image& image, sf::IntRect rect, sf::Image& mask, sf::Color alphaColor = sf::Color::Transparent);

void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect);
void copyImageWithAlpha(sf::Image& dst, sf::Image& src, sf::IntRect srcRect, sf::Color alphaColor = sf::Color::Transparent);
void copyImageWithMask(sf::Image& dst, sf::Image& src, int dstX, int dstY, int srcX, int srcY, sf::Image& mask, sf::Color alphaColor);

void pasteImageWithAlpha(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Color alphaColor = sf::Color::Transparent);
void pasteImageWithMask(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor = sf::Color::Transparent);

enum class SelectionState { None, Selecting, Selected, Moving, Resizing };

class Selection {
public:
	SelectionState _state;

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

	// rect, mask and shader
	sf::IntRect _rect;
	std::shared_ptr<sf::Image> _maskImage;
	sf::Shader _shader;

	// resized things
	sf::IntRect _resizedRect;
	std::shared_ptr<sf::Image> _resizedMaskImage;
	std::shared_ptr<sf::Image> _resizedImage;

	// edge points
	std::vector<std::shared_ptr<EdgePoint>> _edgePoints;	// points use to resize of canvas
	std::shared_ptr<EdgePoint> _point_left_top;
	std::shared_ptr<EdgePoint> _point_top;
	std::shared_ptr<EdgePoint> _point_right_top;
	std::shared_ptr<EdgePoint> _point_left;
	std::shared_ptr<EdgePoint> _point_right;
	std::shared_ptr<EdgePoint> _point_left_bottom;
	std::shared_ptr<EdgePoint> _point_bottom;
	std::shared_ptr<EdgePoint> _point_right_bottom;
	std::shared_ptr<EdgePoint> _hoveredEdgePoint;
	std::shared_ptr<EdgePoint> _clickedEdgePoint;
	sf::Vector2i _orginalEdgePointPosition;

	Selection();
	~Selection();

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
	void generateEdgePoints();
	void resize();
	void drawImage(sf::Vector2i canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor, bool useMask = false);
	void drawOutline(sf::Vector2i canvasPosition, float scale);
	void drawRect(sf::Vector2i canvasPosition, float scale);
	void draw(sf::Vector2i canvasPosition, sf::Vector2i canvasSize, float scale, sf::Color alphaColor);
};

extern std::shared_ptr<Selection> selection;
