#pragma once
#include "SFML/Graphics.hpp"

enum class BrushType { Square, Circle };

class Brush {
public:
    BrushType type;
    int size;
    sf::Vector2i pos_px;

    Brush(int size);
    ~Brush();

    void setBrushType(BrushType type);
    void setSize(int size);
    void decrease();
    void increase();
    void setPosition(sf::Vector2i pos_px);
    std::vector<std::vector<bool>> getBrush();
    void draw(sf::Vector2f canvas_position, sf::Vector2i canvas_size, float zoom, float zoom_delta);
};

extern Brush* brush;