#include "Tools/ResizableTool.hpp"
#include "Time.hpp"
#include "Theme.hpp"
#include "Components/Canvas.hpp"
#include "Cursor.hpp"
#include "WorldToTileConverter.hpp"
#include "DebugLog.hpp"
#include "Dialogs/Dialog.hpp"

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

void ResizableTool::generateImage() {
	_image = std::make_shared<sf::Image>();
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

bool ResizableTool::clickOnSelection(sf::Vector2i point) {

	if (_rect.size.x <= 1 || _rect.size.y <= 1)
		return false;

	return _rect.contains(point);
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
			_state == ResizableToolState::Selecting ||
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

void ResizableTool::drawImage() {
	if (!_image)
		return;

	sf::Texture texture(*_image);
	_sprite = std::make_shared<sf::Sprite>(texture);
	
	float scale = canvas->_zoom * canvas->_zoom_delta;
	float sx = float(_rect.size.x) / float(_image->getSize().x) * scale;
	float sy = float(_rect.size.y) / float(_image->getSize().y) * scale;
	_sprite->setScale(sf::Vector2f(sx, sy));

	sf::IntRect canvasRect(sf::Vector2i(0, 0), canvas->_size);
	auto intersection = _rect.findIntersection(canvasRect);
	if (!intersection.has_value())
		return;

	sf::IntRect inter = intersection.value();
	int tx = std::max(0, -_rect.position.x + inter.position.x);
	int ty = std::max(0, -_rect.position.y + inter.position.y);
	int tw = std::min(int(_image->getSize().x) - tx, inter.size.x);
	int th = std::min(int(_image->getSize().y) - ty, inter.size.y);
	_sprite->setTextureRect(sf::IntRect(sf::Vector2i(tx, ty), sf::Vector2i(tw, th)));

	_sprite->setPosition(sf::Vector2f(inter.position) * scale + sf::Vector2f(canvas->_position));

	window->draw(*_sprite);
}

void ResizableTool::drawEdgePoints() {

	if (!(_points.size() >= 3 && (_state == ResizableToolState::Selected || _state == ResizableToolState::Resizing)))
		return;

	for (auto& point : _edgePoints) {
		point->draw();
	}
}

void ResizableTool::cursorHover() {

	if (!dialogs.empty()) {
		return;
	}

	for (auto& edgePoint : _edgePoints) {
		edgePoint->cursorHover();
	}

	if (_hoveredEdgePoint != nullptr) {
		return;
	}

	if (_rect.contains(cursor->_position)) {
		Element_hovered = this->shared_from_this();
	}
}

void ResizableTool::handleEvent(const sf::Event& event) {
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);

		if (clickOnSelection(tile)) {
			_state = ResizableToolState::Moving;
			_offset = tile - _rect.position;
		}
		else if (canvas->_rect.contains(cursor->_position)) {
			_state = ResizableToolState::Selecting;
			_rect.size = sf::Vector2i(0, 0);
			_points.clear();
			_points.push_back(tile);
			generateRect();
			setPosition(tile);
		}
		else {
			_state = ResizableToolState::None;
			_points.clear();
			generateRect();
			setPosition(tile);
		}
			
	}
	else if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv) {
		if (_state == ResizableToolState::Moving) {
			sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
			sf::Vector2i clampedRectPos = tile - _offset;
			clampedRectPos.x = std::clamp(clampedRectPos.x, - _rect.size.x, canvas->_size.x);
			clampedRectPos.y = std::clamp(clampedRectPos.y, - _rect.size.y, canvas->_size.y);
			setPosition(clampedRectPos);
		}
		else if(_state == ResizableToolState::Selecting) {
			sf::Vector2i tile = worldToTile(cursor->_position, canvas->_position, canvas->_zoom, canvas->_zoom_delta);
			_rect.size = tile - _rect.position;
			sf::Vector2i oldPoint = _points.front();
			_points.clear();
			_points.push_back(oldPoint);
			_points.push_back(oldPoint + sf::Vector2i(tile.x - oldPoint.x, 0));
			_points.push_back(oldPoint + sf::Vector2i(tile.x - oldPoint.x, tile.y - oldPoint.y));
			_points.push_back(oldPoint + sf::Vector2i(0, tile.y - oldPoint.y));
			generateRect();
			generateImage();
		}
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		_state = ResizableToolState::Selected;
		generateEdgePoints();
	}

}

void ResizableTool::update() {
	if (_state == ResizableToolState::Resizing) {
		for (auto& point : _edgePoints) {
			point->update();
		}
		generateRect();
		generateImage();
		return;
	}
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

	if(_rect.size.x <= 1 || _rect.size.y <= 1)
		return;

	
	
	drawImage();
	drawRect();
	drawEdgePoints();
}

std::shared_ptr<ResizableTool> resizable_tool;
