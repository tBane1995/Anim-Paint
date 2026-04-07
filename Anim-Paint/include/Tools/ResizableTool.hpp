#pragma once
#include "SFML/Graphics.hpp"
#include "Element.hpp"
#include "Controls/EdgePoint.hpp"

void pasteImageWithAlpha(sf::Image& dst, sf::Image& src, int dstX, int dstY, sf::Color alphaColor = sf::Color::Transparent);

enum class ResizableToolState { None, Selecting, Selected, Moving, Resizing };

class ResizableTool : public Element {
public:
	ResizableToolState _state;

	// rect and points
	sf::IntRect _rect;
	std::vector<sf::Vector2i> _points;

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

	virtual void addPoint(sf::Vector2i point);
	virtual bool pointOnSegment(sf::Vector2i p, sf::Vector2i a, sf::Vector2i b);
	virtual bool isPointInPolygon(sf::Vector2i p, std::vector<sf::Vector2i>& poly);
	virtual void generateRect();
	virtual void generateImage();
	virtual void generateEdgePoints();
	virtual void resizeRect();
	bool clickOnSelection(sf::Vector2i point);
	virtual void setPosition(sf::Vector2i position);

	virtual void drawRect();
	void drawImage();
	virtual void drawEdgePoints();

	bool tooltypeIsShape();

	virtual void cursorHover();
	virtual void handleEvent(const sf::Event& event);
	virtual void update();
	virtual void draw();
};

extern std::shared_ptr<ResizableTool> resizable_tool;
