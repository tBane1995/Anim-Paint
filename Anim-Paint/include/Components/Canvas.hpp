#pragma once
#include "Element.hpp"
#include "Animation/Frame.hpp"
#include "Controls/EdgePoint.hpp"

enum class CanvasState { Idle, Moving, Resizing };

class Canvas : public Element {
public:

	// background
	sf::Image _bg_image;
	sf::Texture _bg_texture;
	sf::IntRect _rect;

	static sf::Vector2i _minSize;
	static sf::Vector2i _maxSize;
	static sf::Vector2i _size;
	sf::Vector2i _position;

	static int _pixelSize;

	static float _zoom;		// TO_DO in percentage 
	static float _zoom_delta;	// const
	static float _min_zoom;
	static float _max_zoom;

	bool _isEdited;		// if brush/eraser/fill was used, then true
	
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
	sf::Vector2i _topLeftPointPosition;
	std::vector<std::shared_ptr<Frame>> _backupFrames;

	CanvasState _state;
	sf::Vector2i _offset;	// to movements of canvas

	sf::Vector2i _coords;

	Canvas(sf::Vector2i coords = sf::Vector2i(0,0));
	~Canvas();

	void reset();
	bool pointInCanvas(sf::Vector2i point);
	sf::Vector2i getZoomedSize(sf::Vector2i size);
	void setCenter();
	void resize(sf::Vector2i newSize);
	void generateBackground(sf::Vector2i size);
	void generateEdgePoints();
	void setPosition(sf::Vector2i position);
	void setZoom(float mouseWheelScrolllDelta);
	void resize(std::shared_ptr<EdgePoint> edgePoint, sf::Vector2i cursorPosition);
	void setGenerateBackgroundAllCanvases(sf::Vector2i newSize);
	void setResizeAllCanvases(sf::Vector2i newSize);
	void setPositionAllCanvases(sf::Vector2i position);

	int wrap(int v, int size);
	void drawPixels(sf::Color color);
	void fill(sf::Color colorToEdit, sf::Color newColor, sf::Vector2i pixelCoords);
	void fillPixels(sf::Color color);
	void pickPixel();

	void mouseLeftButtonPressedEvent();
	void mouseRightButtonPressedEvent();
	void mouseLeftButtonReleasedEvent();
	void mouseRightButtonReleasedEvent();
	void mouseMovedWithLeftButtonPressedEvent();
	void mouseMovedWithRightButtonPressedEvent();

	void cursorHover();
	void handleEvent(const sf::Event& event);
	float clampAxisOverscroll(float v, float content, float viewport, float overscrollRatio = 0.5f);
	void update();
	void draw();

};

extern std::shared_ptr<Canvas> canvas;
extern std::vector<std::shared_ptr<Canvas>> canvases;
