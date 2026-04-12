#include "Tools/Brush.hpp"
#include "Window.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Components/Canvas.hpp"
#include "Cursor.hpp"
#include "Components/MainMenu/MainMenu.hpp"

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

void Brush::draw() {

    std::vector<std::vector<bool>> brush = getBrush();
    float scale = canvas->_zoom * canvas->_zoom_delta;

    const int h = static_cast<int>(brush.size());
    const int w = h ? static_cast<int>(brush[0].size()) : 0;
    const int halfX = w / 2;
    const int halfY = h / 2;

    for (auto& canvas : canvases) {

        // single
        if (main_menu->canvas_repeating->_checkbox->_value == 0 && !(canvas->_coords.x == 0 && canvas->_coords.y == 0)) {
            continue;
        }

        // cross
        if (main_menu->canvas_repeating->_checkbox->_value == 1 && !(canvas->_coords.x == 0 || canvas->_coords.y == 0)) {
            continue;
        }

        for (int y = 0; y < h; ++y) {
            for (int x = 0; x < w; ++x) {
                if (!brush[y][x]) continue;


                int tx = _position.x - halfX + x;
                int ty = _position.y - halfY + y;
                
                int cx = std::floor((float)tx / Canvas::_size.x);
                int cy = std::floor((float)ty / Canvas::_size.y);

                // single - only draw on the main canvas
                if (main_menu->canvas_repeating->_checkbox->_value == 0 && !(cx == 0 && cy == 0)) {
                    continue;
                }

                // cross - only  on the main canvas
                if (main_menu->canvas_repeating->_checkbox->_value == 1 && !(cx == 0 || cy == 0)) {
                    continue;
                }

                tx = (tx % Canvas::_size.x + Canvas::_size.x) % Canvas::_size.x;
                ty = (ty % Canvas::_size.y + Canvas::_size.y) % Canvas::_size.y;

				//std::wcout << tx << L", " << ty << L"\n";
                sf::RectangleShape rect(sf::Vector2f(scale, scale));
                rect.setFillColor(toolbar->_first_color->_color);
                rect.setPosition(sf::Vector2f(canvas->_position) + sf::Vector2f(tx * scale, ty * scale));  

                window->draw(rect);
            }
        }
    }
}

std::shared_ptr<Brush> brush;