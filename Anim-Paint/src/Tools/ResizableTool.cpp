#include "Tools/ResizableTool.hpp"
#include "Time.hpp"
#include "Theme.hpp"
#include "Components/Canvas.hpp"
#include "Cursor.hpp"
#include "WorldToTileConverter.hpp"
#include "DebugLog.hpp"


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

void ResizableTool::generateRect() {

	if (_points.empty()) {
		_rect = sf::IntRect({ 0,0 }, { 0,0 });
		return;
	}

	int minX = std::numeric_limits<int>::max();
	int minY = std::numeric_limits<int>::max();
	int maxX = std::numeric_limits<int>::min();
	int maxY = std::numeric_limits<int>::min();

	for (auto& p : _points) {
		minX = std::min(minX, p.x);
		minY = std::min(minY, p.y);
		maxX = std::max(maxX, p.x);
		maxY = std::max(maxY, p.y);
	}

	_rect = sf::IntRect(
		sf::Vector2i(minX, minY),
		sf::Vector2i(maxX - minX + 1, maxY - minY + 1)
	);
}

void ResizableTool::generateOutline(bool selectionComplete) {

	if (_points.size() < 1) return;
	if (std::abs(_rect.size.x) <= 1 || std::abs(_rect.size.y) <= 1) return;

	if (!_outlineRenderTexture.resize(sf::Vector2u(std::abs(_rect.size.x), std::abs(_rect.size.y)))) {
		DebugError(L"Lasso::generateOutline: Failed to resize outline render texture.");
		exit(0);
	}

	// calc the shift to move points to (0,0) based coordinates for rendering
	int minX = std::numeric_limits<int>::max();
	int minY = std::numeric_limits<int>::max();
	for (const auto& pt : _points) {
		minX = std::min(minX, pt.x);
		minY = std::min(minY, pt.y);
	}

	sf::Vector2f shift = sf::Vector2f((float)(-minX), (float)(-minY));

	// generate outline texture
	_outlineRenderTexture.clear(sf::Color(0, 0, 0, 0));
	sf::Color selectionColor = selection_border_color;
	sf::VertexArray lines(sf::PrimitiveType::LineStrip);

	for (auto& point : _points)
		lines.append(sf::Vertex{ sf::Vector2f(point) + shift, selectionColor });

	// first and last point 
	sf::VertexArray p(sf::PrimitiveType::Points);
	p.append(sf::Vertex{ sf::Vector2f(_points.front()) + shift, selectionColor });
	p.append(sf::Vertex{ sf::Vector2f(_points.back()) + shift, selectionColor });

	if (selectionComplete) lines.append(sf::Vertex{ sf::Vector2f(_points.front()) + shift, selectionColor });

	sf::RenderStates rs;
	rs.blendMode = sf::BlendAlpha;
	rs.transform.translate(sf::Vector2f(0.5f, 0.5f));
	_outlineRenderTexture.draw(lines, rs);
	_outlineRenderTexture.draw(p, rs);
	_outlineRenderTexture.display();

}

void ResizableTool::generateEdgePoints() {

	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x) * scale;
	rectSize.y = float(_rect.size.y) * scale;

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

void ResizableTool::setPosition(sf::Vector2i position) {
	_rect.position = position;

	float scale = canvas->_zoom * canvas->_zoom_delta;

	sf::Vector2f rectSize;
	rectSize.x = float(_rect.size.x) * scale;
	rectSize.y = float(_rect.size.y) * scale;

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
}

void ResizableTool::drawRect() {
	if (!
		(_points.size() >= 3 &&
			(
			_state == ResizableToolState::Selected ||
			_state == ResizableToolState::Moving ||
			_state == ResizableToolState::Resizing)))
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

	rect.setFillColor(selection_color);
	rect.setOutlineColor(selection_border_color);
	rect.setOutlineThickness((float)(selection_border_width));

	window->draw(rect);
}

void ResizableTool::drawOutline() {

	if (_state != ResizableToolState::Selecting)
		return;

	if (_rect.size.x <= 1 || _rect.size.y <= 1)
		return;

	float scale = canvas->_zoom * canvas->_zoom_delta;

	_outlineSprite = std::make_shared<sf::Sprite>(_outlineRenderTexture.getTexture());
	_outlineSprite->setScale(sf::Vector2f(scale, scale));

	sf::Vector2f outlineSpritePos;
	outlineSpritePos.x = (float)(canvas->_position.x) + (float)(_rect.position.x) * scale;
	outlineSpritePos.y = (float)(canvas->_position.y) + (float)(_rect.position.y) * scale;
	_outlineSprite->setPosition(outlineSpritePos);

	window->draw(*_outlineSprite);

}

void ResizableTool::drawEdgePoints() {

	if (!(_points.size() >= 3 && (_state == ResizableToolState::Selected || _state == ResizableToolState::Resizing)))
		return;


	for (auto& point : _edgePoints) {
		point->draw();
	}
}

void ResizableTool::cursorHover() {

	
}

void ResizableTool::handleEvent(const sf::Event& event) {
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		

		if (canvas->_rect.contains(cursor->_position)) {
			_state = ResizableToolState::Selecting;
			sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
			_rect.position = tile;
			_rect.size = sf::Vector2i(0, 0);
			_points.clear();
			_points.push_back(tile);
			generateRect();
			generateOutline();
			generateEdgePoints();
		}
			
	}

	if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv) {
		if(_state == ResizableToolState::Selecting) {
			sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
			_rect.size = tile - _rect.position;
			sf::Vector2i oldPoint = _points.front();
			_points.clear();
			_points.push_back(oldPoint);
			_points.push_back(oldPoint + sf::Vector2i(tile.x - oldPoint.x, 0));
			_points.push_back(oldPoint + sf::Vector2i(tile.x - oldPoint.x, tile.y - oldPoint.y));
			_points.push_back(oldPoint + sf::Vector2i(0, tile.y - oldPoint.y));
			generateRect();
			generateOutline(true);
			generateEdgePoints();
		}
	}

	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		_state = ResizableToolState::Selected;
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

	if (_state == ResizableToolState::None)
		return;

	
	drawRect();
	drawOutline();
	drawEdgePoints();
}

std::shared_ptr<ResizableTool> resizableTool;
