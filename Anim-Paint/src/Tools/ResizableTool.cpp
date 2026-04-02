#include "Tools/ResizableTool.hpp"
#include "Time.hpp"
#include "Theme.hpp"
#include "Components/Canvas.hpp"
#include "Cursor.hpp"
#include "WorldToTileConverter.hpp"

ResizableTool::ResizableTool() : Element() {
	_state = ResizableToolState::None;

	_rect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(0, 0));
	_points.clear();

	_offset = sf::Vector2i(0, 0);
	_moveTime = currentTime;
}

ResizableTool::~ResizableTool() {

}


void ResizableTool::addPoint(sf::Vector2i point) {

	if (_points.empty()) {
		_points.push_back(point);
		return;
	}

	if (_points.back() == point) return;

	_points.push_back(point);
}

bool ResizableTool::pointOnSegment(sf::Vector2i p, sf::Vector2i a, sf::Vector2i b)
{
	int cross = 1LL * (b.x - a.x) * (p.y - a.y) - 1LL * (b.y - a.y) * (p.x - a.x);

	if (cross != 0)
		return false;

	int minx = std::min(a.x, b.x);
	int maxx = std::max(a.x, b.x);
	int miny = std::min(a.y, b.y);
	int maxy = std::max(a.y, b.y);

	return(p.x >= minx && p.x <= maxx && p.y >= miny && p.y <= maxy);
}

bool ResizableTool::isPointInPolygon(sf::Vector2i p, std::vector < sf::Vector2i >& poly)
{
	size_t n = poly.size();
	if (n < 3)
		return false;

	bool inside = false;
	for (size_t i = 0, j = n - 1; i < n; j = i++) {
		sf::Vector2i& a = poly[j];
		sf::Vector2i& b = poly[i];

		if (pointOnSegment(p, a, b))
			return true;

		bool crossesY = ((a.y > p.y) != (b.y > p.y));
		if (!crossesY)
			continue;

		int dy = b.y - a.y;
		int lhs = (p.x - a.x) * dy;
		int rhs = (b.x - a.x) * (p.y - a.y);

		bool hit = (dy > 0) ? (lhs < rhs)
			: (lhs > rhs);
		if (hit) inside = !inside;

	}
	return inside;
}

void ResizableTool::generateEdgePoints() {

	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x);
	rectSize.y = float(_rect.size.y);

	sf::Vector2f rectPos;
	rectPos.x = float(canvas->_position.x) + float(_rect.position.x) * scale;
	rectPos.y = float(canvas->_position.y) + float(_rect.position.y) * scale;

	float m = (float)(selection_border_width);

	_edgePoints.clear();
	_point_left_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(-m)));
	_point_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x / 2), (int)(-m)));
	_point_right_top = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(-m)));

	_point_left = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(rectSize.y / 2)));
	_point_right = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(rectSize.y / 2)));

	_point_left_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(-m), (int)(rectSize.y + m)));
	_point_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x / 2), (int)(rectSize.y + m)));
	_point_right_bottom = std::make_shared<EdgePoint>(sf::Vector2i(rectPos) + sf::Vector2i((int)(rectSize.x + m), (int)(rectSize.y + m)));

	_edgePoints.push_back(_point_left_top);
	_edgePoints.push_back(_point_top);
	_edgePoints.push_back(_point_right_top);
	_edgePoints.push_back(_point_left);
	_edgePoints.push_back(_point_right);
	_edgePoints.push_back(_point_left_bottom);
	_edgePoints.push_back(_point_bottom);
	_edgePoints.push_back(_point_right_bottom);

}

void ResizableTool::cursorHover() {

	
}

void ResizableTool::handleEvent(const sf::Event& event) {
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		_rect.position = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
		_rect.size = sf::Vector2i(0, 0);
	}

	if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv) {
		sf::Vector2i currentTile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
		_rect.size = currentTile - _rect.position;
	}

}

void ResizableTool::update() {

}

void ResizableTool::draw() {

	if (!(
		toolbar->_toolType == ToolType::Circle ||
		toolbar->_toolType == ToolType::Triangle ||
		toolbar->_toolType == ToolType::Rectangle ||
		toolbar->_toolType == ToolType::Diamond ||
		toolbar->_toolType == ToolType::Pentagon ||
		toolbar->_toolType == ToolType::HexagonFlatTop ||
		toolbar->_toolType == ToolType::HexagonPointTop ||
		toolbar->_toolType == ToolType::Octagon
		))
		return;

	if (!sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
		return;

	
	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x) * scale;
	rectSize.y = float(_rect.size.y) * scale;

	sf::RectangleShape rect(rectSize);

	sf::Vector2f rectPos;
	rectPos.x = float(canvas->_position.x) + float(_rect.position.x) * scale;
	rectPos.y = float(canvas->_position.y) + float(_rect.position.y) * scale;
	rect.setPosition(rectPos);

	rect.setPosition(sf::Vector2f(rectPos));
	rect.setFillColor(sf::Color::Red);

	window->draw(rect);
}

std::shared_ptr<ResizableTool> resizableTool;
