#pragma once
#include "SFML/Graphics.hpp"

enum class BrushType { Square, Circle };

class Brush {
public:
    BrushType _type;
    int _size;
    sf::Vector2i _position;

    Brush(int size);
    ~Brush();

    void setBrushType(BrushType type);
    void setSize(int size);
    void decrease();
    void increase();
    void setPosition(sf::Vector2i position);
    std::vector<std::vector<bool>> getBrush();
    void draw(sf::Vector2i canvas_position, sf::Vector2i canvas_size, float zoom, float zoom_delta);
};

extern std::shared_ptr<Brush> brush;