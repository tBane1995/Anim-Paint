#ifndef Selection_hpp
#define Selection_hpp

class Selection {
public:
	sf::Vector2i start_px, end_px; // piksele obrazu (0..size.x/y)
	sf::Vector2i offset;
	bool isMoved;

	Selection() {
		start_px = sf::Vector2i(0, 0);
		end_px = sf::Vector2i(0, 0);
		offset = sf::Vector2i(0, 0);
		isMoved = false;
	}

	void setOffset(sf::Vector2i point_px) {
		sf::Vector2i s(std::min(start_px.x, end_px.x), std::min(start_px.y, end_px.y));
		offset = s - point_px;
	}

	void move(sf::Vector2i point_px, sf::Vector2u map_size) {
		sf::Vector2i s(std::min(start_px.x, end_px.x), std::min(start_px.y, end_px.y));
		sf::Vector2i e(std::max(start_px.x, end_px.x), std::max(start_px.y, end_px.y));
		sf::Vector2i sz = e - s;                 // szerokość/wysokość w pikselach

		sf::Vector2i newMin = point_px + offset;

		start_px = newMin;
		end_px = newMin + sz;

		if (start_px.x < 0) {
			start_px.x = 0;
			end_px.x = sz.x;
		}

		if (start_px.y < 0) {
			start_px.y = 0;
			end_px.y = sz.y;
		}

		if (end_px.x >= map_size.x) {
			end_px.x = map_size.x;
			start_px.x = end_px.x - sz.x;
		}

		if (end_px.y >= map_size.y) {
			end_px.y = map_size.y;
			start_px.y = end_px.y - sz.y;
		}

		//std::cout << start_px.x << ", " << start_px.y << "\n";
	}

	bool clickOnSelection(sf::Vector2i p) const {
		sf::Vector2i s(std::min(start_px.x, end_px.x), std::min(start_px.y, end_px.y));
		sf::Vector2i e(std::max(start_px.x, end_px.x), std::max(start_px.y, end_px.y));
		return (p.x > s.x && p.x < e.x && p.y > s.y && p.y < e.y);
	}

	void draw(const sf::Vector2f& canvasPos, float scale) {
		sf::Vector2i s(std::min(start_px.x, end_px.x), std::min(start_px.y, end_px.y));
		sf::Vector2i e(std::max(start_px.x, end_px.x), std::max(start_px.y, end_px.y));

		sf::Vector2f topLeft = canvasPos + sf::Vector2f(s.x * scale, s.y * scale);
		sf::Vector2f size = sf::Vector2f((e.x - s.x) * scale, (e.y - s.y) * scale);

		sf::RectangleShape rect(size);
		rect.setPosition(topLeft);
		rect.setFillColor(sf::Color(255, 47, 47, 127));
		window->draw(rect);
	}
};

Selection* selection = nullptr;
#endif