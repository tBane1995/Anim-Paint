#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "Animation/Frame.hpp"

enum class CanvasState { Idle, Moving, Resizing };

// TO-DO - Edge Point
class EdgePoint : public ElementGUI {
public:
	sf::IntRect _rect;

	EdgePoint(sf::Vector2i position);
	~EdgePoint();

	void setPosition(sf::Vector2i position);
	sf::Vector2i getPosition();
	sf::Vector2i getSize();
	void cursorHover();
	void handleEvent(const sf::Event& event);
	void update();
	void draw();
};

class Canvas : public ElementGUI {
public:

	// background
	sf::Image _bg_image;
	sf::Texture _bg_texture;
	sf::IntRect _rect;

	sf::Vector2i _minSize;
	sf::Vector2i _maxSize;
	sf::Vector2i _size;
	sf::Vector2i _position;

	int _pixelSize;

	float _zoom;		// in percentage
	float _zoom_delta;	// const
	float _min_zoom;
	float _max_zoom;

	sf::Vector2f _edgePointSize;
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
	std::vector<std::shared_ptr<Frame>> _backupFrames;

	CanvasState _state;
	sf::Vector2i _offset;	// to movements of canvas

	Canvas();
	~Canvas();

	void resize(sf::Vector2i size);
	bool pointInCanvas(sf::Vector2i point);
	sf::Vector2i getZoomedSize(sf::Vector2i size);
	void generateBackground(sf::Vector2i size);
	void generateEdgePoints();
	void setPosition(sf::Vector2i position);
	void setCenter();
	void setZoom(float mouseWheelScrolllDelta);
	void drawPixels(sf::Color color);
	void fill(sf::Color colorToEdit, sf::Color newColor, sf::Vector2i pixelCoords);
	void fillPixels(sf::Color color);
	void pickPixel();

	void mouseLeftButtonPressedEvent();
	void mouseRightButtonPressedEvent();
	void mouseLeftButtonReleasedEvent();
	void mouseMovedWithLeftButtonPressedEvent();
	void mouseMovedWithRightButtonPressedEvent();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	float clampAxisOverscroll(float v, float content, float viewport, float overscrollRatio = 0.5f);
	void update();
	void draw();

};

extern std::shared_ptr<Canvas> canvas;
