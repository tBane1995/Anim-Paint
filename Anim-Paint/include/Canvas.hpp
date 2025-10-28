#pragma once
#include "ElementGUI/ElementGUI.hpp"
#include "Animation/Frame.hpp"

enum class CanvasState { Idle, Moving, Resizing };

class Canvas : public ElementGUI {
public:

	// background
	sf::Image _bg_image;
	sf::Texture _bg_texture;
	sf::Sprite* _bg_sprite;

	sf::Vector2i _minSize;
	sf::Vector2i _maxSize;
	sf::Vector2i _size;
	sf::Vector2f _position;

	//std::vector < sf::Texture* > canvas_textures;
	//std::vector < sf::Sprite > canvas_sprites;

	int _pixelSize;

	float _zoom;			// in percentage
	float _zoom_delta;	// const
	float _min_zoom;
	float _max_zoom;

	sf::Vector2f _edgePointSize;
	std::vector < sf::RectangleShape* > _edgePoints;	// points use to resize of canvas
	sf::RectangleShape* _point_left_top;
	sf::RectangleShape* _point_top;
	sf::RectangleShape* _point_right_top;
	sf::RectangleShape* _point_left;
	sf::RectangleShape* _point_right;
	sf::RectangleShape* _point_left_bottom;
	sf::RectangleShape* _point_bottom;
	sf::RectangleShape* _point_right_bottom;
	sf::RectangleShape* _hoveredEdgePoint;
	sf::RectangleShape* _clickedEdgePoint;
	sf::Vector2f _orginalEdgePointPosition;
	std::vector < Frame* > _backupFrames;

	CanvasState _state;
	sf::Vector2f _offset;	// to movements of canvas

	bool _brush_is_visible;

	Canvas();
	~Canvas();

	void resize(sf::Vector2i size);
	sf::Vector2f getZoomedSize(sf::Vector2i size);
	void generateBackground(sf::Vector2i size);
	void generateEdgePoints();
	void setPosition(sf::Vector2f position);
	void setZoom(float mouseWheelScrolllDelta);
	void drawPixels(sf::Color color);
	void fill(sf::Color colorToEdit, sf::Color newColor, sf::Vector2i pixelCoords);
	void fillPixels(sf::Color color);

	void cursorHover();
	void handleEvent(const sf::Event& event);
	float clampAxisOverscroll(float v, float content, float viewport, float overscrollRatio = 0.5f);
	void update();
	void draw();

};

extern Canvas* canvas;
