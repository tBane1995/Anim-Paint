#pragma once
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "Controls/EdgePoint.hpp"


enum class ResizableToolState { None, Selecting, Selected, Moving, Resizing };

class ResizableTool : public Element {
public:
	ResizableToolState _state;

	// rect and points
	sf::IntRect _rect;
	std::vector<sf::Vector2i> _points;

	// outline
	sf::RenderTexture _outlineRenderTexture;
	std::shared_ptr<sf::Sprite> _outlineSprite;

	// image
	std::shared_ptr<sf::Image> _image;
	sf::Texture _texture;
	std::shared_ptr<sf::Sprite> _sprite;

	// others
	sf::Vector2i _offset;	// to move
	sf::Time _moveTime;		// to limit selection move

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

	ResizableTool();
	~ResizableTool();

	void addPoint(sf::Vector2i point);
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

extern std::shared_ptr<ResizableTool> resizableTool;
