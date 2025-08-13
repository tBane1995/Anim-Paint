#ifndef Brush_hpp
#define Brush_hpp

std::vector<std::vector<std::vector<bool>>> brushes = {

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

class Brush {
public:
    sf::Vector2i pos_px;
    int size;

    Brush(int size) {
        pos_px = sf::Vector2i(0, 0);
        setSize(size);
    }

    ~Brush() {

    }

    void setSize(int size) {
        this->size = size;
    }

    void setPosition(sf::Vector2i pos_px) {
        this->pos_px = pos_px;
    }

    void draw(sf::Vector2f canvas_position, sf::Vector2i canvas_size, float zoom, float zoom_delta) {
        const auto& brush = brushes[size];
        const float scale = zoom * zoom_delta;

        const int h = static_cast<int>(brush.size());
        const int w = h ? static_cast<int>(brush[0].size()) : 0;
        const int halfX = w / 2;
        const int halfY = h / 2;

        // bazowa pozycja lewego-górnego kafelka pędzla w tej samej skali
        sf::Vector2f brush_pos = canvas_position +
            sf::Vector2f((pos_px.x - halfX) * scale, (pos_px.y - halfY) * scale);

        sf::RectangleShape rect({ scale, scale });
        rect.setFillColor(sf::Color::Red);

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