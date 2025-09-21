#pragma once
#include "ElementGUI.hpp"
#include "Frame.hpp"

enum class CanvasState { Idle, Moving, Resizing };

class Canvas : public ElementGUI {
public:

	// background
	sf::Image bg_image;
	sf::Texture bg_texture;
	sf::Sprite bg_sprite;

	sf::Vector2i size;
	sf::Vector2f position;

	//std::vector < sf::Texture* > canvas_textures;
	//std::vector < sf::Sprite > canvas_sprites;

	int pixelSize;

	float zoom;			// in percentage
	float zoom_delta;	// const
	float min_zoom;
	float max_zoom;

	sf::Vector2f edgePointSize;
	std::vector < sf::RectangleShape* > edgePoints;	// points use to resize of canvas
	sf::RectangleShape* point_left_top;
	sf::RectangleShape* point_top;
	sf::RectangleShape* point_right_top;
	sf::RectangleShape* point_left;
	sf::RectangleShape* point_right;
	sf::RectangleShape* point_left_bottom;
	sf::RectangleShape* point_bottom;
	sf::RectangleShape* point_right_bottom;
	sf::RectangleShape* hoveredEdgePoint;
	sf::RectangleShape* clickedEdgePoint;
	sf::Vector2f orginalEdgePointPosition;
	std::vector < Frame* > backupFrames;

	CanvasState state;
	sf::Vector2f offset;	// to movements of canvas

	bool brush_is_visible;

	Canvas(sf::Vector2i size);
	~Canvas();

	sf::Vector2f getZoomedSize(sf::Vector2i size);
	void generateBackground(sf::Vector2i size);
	void generateEdgePoints();
	void setPosition(sf::Vector2f position);
	void setZoom(float mouseWheelScrolllDelta);
	void drawPixels(sf::Color color);
	void fill(sf::Color colorToEdit, sf::Color newColor, sf::Vector2i pixelCoords);
	void fillPixels(sf::Color color);

	void cursorHover();
	void handleEvent(sf::Event& event);
	float clampAxisOverscroll(float v, float content, float viewport, float overscrollRatio = 0.5f);
	void update();
	void draw();

};

extern Canvas* canvas;
