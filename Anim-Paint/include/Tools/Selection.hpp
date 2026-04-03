#pragma once
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "Controls/EdgePoint.hpp"

void removeImageWithAlpha(sf::Image& image, sf::IntRect rect, sf::Color alphaColor = sf::Color::Transparent);
void removeImageWithMask(sf::Image& image, sf::IntRect rect, sf::Image& mask, sf::Color alphaColor = sf::Color::Transparent);

void copyImage(sf::Image& dst, sf::Image& src, sf::IntRect srcRect);
void copyImageWithAlpha(sf::Image& dst, sf::Image& src, sf::IntRect srcRect, sf::Color alphaColor = sf::Color::Transparent);
void copyImageWithMask(sf::Image& dst, sf::Image& src, int dstX, int dstY, int srcX, int srcY, sf::Image& mask, sf::Color alphaColor);

void pasteImageWithAlpha(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Color alphaColor = sf::Color::Transparent);
void pasteImageWithMask(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor = sf::Color::Transparent);

enum class SelectionState { None, Selecting, Selected, Moving, Resizing };

class Selection : public Element {
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
	sf::Shader _maskShader;

	// resized things
	sf::IntRect _resizedRect;
	std::shared_ptr<sf::Image> _resizedMaskImage;
	sf::Shader _resizedMaskShader;
	std::shared_ptr<sf::Image> _resizedImage;

	// edge points
	std::vector<std::shared_ptr<EdgePoint>> _edgePoints;
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

	sf::Time _moveTime; // to limit selection move

	Selection();
	~Selection();

	
	void unselect();
	void selectAll();
	bool clickOnSelection(sf::Vector2i point);
	void copy(sf::Image& canvas, sf::Color alphaColor);
	void paste(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Image& mask, sf::Color alphaColor);
	bool paste(sf::Image& canvas, sf::Color emptyColor, sf::Image image);
	void cut(sf::Image& canvas, sf::Color emptyColor);
	
	void generateMask();
	void generateResizedMask();
	void resizeRect();
	void resizeImage();
	void drawImage(bool useMask = false);
	void drawResizedImage(bool useMask = false);

	void shiftOriginIfNeeded(sf::Vector2i& point);
	void addPoint(sf::Vector2i point);
	void normalize(sf::IntRect newRectSize);
	bool pointOnSegment(sf::Vector2i p, sf::Vector2i a, sf::Vector2i b);
	bool isPointInPolygon(sf::Vector2i p, std::vector<sf::Vector2i>& poly);
	void generateRect();
	void generateOutline(bool selectionComplete = false);
	void generateEdgePoints();
	void setPosition(sf::Vector2i position);

	void drawRect();
	void drawOutline();
	void drawEdgePoints();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

extern std::shared_ptr<Selection> selection;
