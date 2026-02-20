#include "Tools/Brush.hpp"
#include "Window.hpp"
#include "Toolbar.hpp"

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
        {1, 1, 1, 1, 1, 1, 1}
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

Brush::Brush(int size) {
    _type = BrushType::Circle;
    setSize(size);
    _lastPosition = sf::Vector2i(-1, -1);
    _position = sf::Vector2i(0, 0);
}

Brush::~Brush() {

}

void Brush::setBrushType(BrushType type) {
    _type = type;
}

void Brush::setSize(int size) {
    _size = size;
}

void Brush::decrease() {
    _size--;
    if (_size < 0)
        _size = 0;
}

void Brush::increase() {
    _size++;
    if (_size > 5)
        _size = 5;
}

void Brush::setPosition(sf::Vector2i position) {
	_lastPosition = _position;
    _position = position;
}

std::vector<std::vector<bool>> Brush::getBrush() {

    if (_type == BrushType::Square) {
        return square_brushes[_size];
    }
    else {
        return circle_brushes[_size];
    }
}

void Brush::draw(sf::Vector2i canvas_position, sf::Vector2i canvas_size, float zoom, float zoom_delta) {
    std::vector<std::vector<bool>> brush = getBrush();


    const float scale = zoom * zoom_delta;

    const int h = static_cast<int>(brush.size());
    const int w = h ? static_cast<int>(brush[0].size()) : 0;
    const int halfX = w / 2;
    const int halfY = h / 2;

    // bazowa pozycja lewego-górnego kafelka pędzla w tej samej skali
    sf::Vector2f brush_pos = sf::Vector2f(canvas_position) + sf::Vector2f((_position.x - halfX) * scale, (_position.y - halfY) * scale);

    sf::RectangleShape rect({ scale, scale });
    rect.setFillColor(toolbar->_first_color->_color);

    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            if (!brush[y][x]) continue;

            const int tx = _position.x - halfX + x;
            const int ty = _position.y - halfY + y;
            if (tx < 0 || ty < 0 || tx >= canvas_size.x || ty >= canvas_size.y) continue;

            rect.setPosition(brush_pos + sf::Vector2f(x * scale, y * scale));
            window->draw(rect);
        }
    }
}

std::shared_ptr<Brush> brush;