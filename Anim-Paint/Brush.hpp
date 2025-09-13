#ifndef Brush_hpp
#define Brush_hpp

std::vector<std::vector<std::vector<bool>>> circle_brushes = {

    // 0 - 1x1
    {
        {1}
    },

    // 1 - 3x3
    {
        {0, 1, 0},
        {1, 1, 1},
        {0, 1, 0}
    },

    // 2 - 5x5
    {
        {0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {0, 1, 1, 1, 0}
    },

    // 3 - 7x7
    {
        {0, 0, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 0, 0}
    },

    // 4 - 9x9
    {
        {0, 0, 0, 1, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 1, 1, 1, 0, 0, 0}
    },

    // 5 - 11x11
    {
        {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0},
        {0, 0, 1, 1, 1, 1, 1, 1, 1, 0, 0},
        {0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0},

    }
};

std::vector<std::vector<std::vector<bool>>> square_brushes = {

    // 0 - 1x1
    {
        {1}
    },

    // 1 - 3x3
    {
        {1, 1, 1},
        {1, 1, 1},
        {1, 1, 1}
    },

    // 2 - 5x5
    {
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1}
    },

    // 3 - 7x7
    {
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 1, 1, 1, 1}
    },

    // 4 - 9x9
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1}
    },

    // 5 - 11x11
    {
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
        {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},

    }
};

enum class BrushType { Square, Circle };

class Brush {
public:
    BrushType type;
    int size;
    sf::Vector2i pos_px;
    
    


    Brush(int size) {
        type = BrushType::Circle;
        setSize(size);
        pos_px = sf::Vector2i(0, 0);
    }

    ~Brush() {

    }

    void setBrushType(BrushType type) {
        this->type = type;
    }

    void setSize(int size) {
        this->size = size;
    }

    void decrease() {
        this->size--;
        if (this->size < 0)
            this->size = 0;
    }

    void increase() {
        this->size++;
        if (this->size > 5)
            this->size = 5;
    }

    void setPosition(sf::Vector2i pos_px) {
        this->pos_px = pos_px;
    }

    std::vector<std::vector<bool>> getBrush() {

        if (type == BrushType::Square) {
            return square_brushes[size];
        }
        else {
            return circle_brushes[size];
        }
    }

    void draw(sf::Vector2f canvas_position, sf::Vector2i canvas_size, float zoom, float zoom_delta) {
        std::vector<std::vector<bool>> brush = getBrush();
        

        const float scale = zoom * zoom_delta;

        const int h = static_cast<int>(brush.size());
        const int w = h ? static_cast<int>(brush[0].size()) : 0;
        const int halfX = w / 2;
        const int halfY = h / 2;

        // bazowa pozycja lewego-górnego kafelka pędzla w tej samej skali
        sf::Vector2f brush_pos = canvas_position +
            sf::Vector2f((pos_px.x - halfX) * scale, (pos_px.y - halfY) * scale);

        sf::RectangleShape rect({ scale, scale });
        rect.setFillColor(sf::Color::Black);

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                if (!brush[y][x]) continue;

                const int tx = pos_px.x - halfX + x;
                const int ty = pos_px.y - halfY + y;
                if (tx < 0 || ty < 0 || tx >= canvas_size.x || ty >= canvas_size.y) continue;

                rect.setPosition(brush_pos + sf::Vector2f(x * scale, y * scale));
                window->draw(rect);
            }
        }
    }
};

Brush* brush = nullptr;

#endif