#include "Canvas.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Mouse.hpp"
#include "Tools/Brush.hpp"
#include "Animation/Animation.hpp"
#include "WorldToTileConverter.hpp"
#include "Tools/Toolbar.hpp"
#include "Tools/Selection.hpp"
#include "Tools/Lasso.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Dialogs/Dialog.hpp"

Canvas::Canvas(sf::Vector2i size) : ElementGUI() {
	this->_size = size;
	this->_pixelSize = 8.0f;


	int targetSize = 64;
	float sides = std::max(size.x, size.y);
	this->_zoom_delta = 16.0f;
	this->_min_zoom = 0.125f;
	this->_max_zoom = 0.5f;
	this->_zoom = targetSize / (sides * 4.0f);


	this->_size = sf::Vector2i(targetSize, targetSize);

	_state = CanvasState::Idle;

	this->_offset = sf::Vector2f(0, 0);

	this->_brush_is_visible = false;

	generateBackground(size);
	generateEdgePoints();

	setPosition((sf::Vector2f(window->getSize()) - getZoomedSize(size)) / 2.0f);
}

Canvas::~Canvas() {

}

sf::Vector2f Canvas::getZoomedSize(sf::Vector2i size) {
	return sf::Vector2f(int(size.x * _zoom_delta * _zoom), int(size.y * _zoom_delta * _zoom));
}

void Canvas::generateBackground(sf::Vector2i size) {

	_bg_image = sf::Image();
	sf::Vector2f s = getZoomedSize(size);
	_bg_image.create(s.x, s.y, sf::Color::Transparent);

	sf::Color c1 = canvas_color1;
	sf::Color c2 = canvas_color2;

	for (int y = 0; y < s.y; y++) {
		for (int x = 0; x < s.x; x++) {

			int xx = x / _pixelSize;
			int yy = y / _pixelSize;

			sf::Color c;

			if (yy % 2 == 0)
				(xx % 2 == 0) ? c = c1 : c = c2;
			else
				(xx % 2 == 1) ? c = c1 : c = c2;

			_bg_image.setPixel(x, y, c); // tu jest błąd - wychodzi poza zakres

		}
	}

	_bg_texture = sf::Texture();
	_bg_texture.loadFromImage(_bg_image);

	_bg_sprite = sf::Sprite(_bg_texture);
}

void Canvas::generateEdgePoints() {
	_edgePoints.clear();
	_edgePointSize = sf::Vector2f(8, 8);

	sf::Color color(128, 47, 47, 255);

	// top points
	_point_left_top = new sf::RectangleShape(_edgePointSize);
	_point_left_top->setOrigin(_edgePointSize / 2.0f);
	_point_left_top->setFillColor(color);
	_edgePoints.push_back(_point_left_top);

	_point_top = new sf::RectangleShape(_edgePointSize);
	_point_top->setOrigin(_edgePointSize / 2.0f);
	_point_top->setFillColor(color);
	_edgePoints.push_back(_point_top);

	_point_right_top = new sf::RectangleShape(_edgePointSize);
	_point_right_top->setOrigin(_edgePointSize / 2.0f);
	_point_right_top->setFillColor(color);
	_edgePoints.push_back(_point_right_top);

	// left & right
	_point_left = new sf::RectangleShape(_edgePointSize);
	_point_left->setOrigin(_edgePointSize / 2.0f);
	_point_left->setFillColor(color);
	_edgePoints.push_back(_point_left);

	_point_right = new sf::RectangleShape(_edgePointSize);
	_point_right->setOrigin(_edgePointSize / 2.0f);
	_point_right->setFillColor(color);
	_edgePoints.push_back(_point_right);

	// bottom points
	_point_left_bottom = new sf::RectangleShape(_edgePointSize);
	_point_left_bottom->setOrigin(_edgePointSize / 2.0f);
	_point_left_bottom->setFillColor(color);
	_edgePoints.push_back(_point_left_bottom);

	_point_bottom = new sf::RectangleShape(_edgePointSize);
	_point_bottom->setOrigin(_edgePointSize / 2.0f);
	_point_bottom->setFillColor(color);
	_edgePoints.push_back(_point_bottom);

	_point_right_bottom = new sf::RectangleShape(_edgePointSize);
	_point_right_bottom->setOrigin(_edgePointSize / 2.0f);
	_point_right_bottom->setFillColor(color);
	_edgePoints.push_back(_point_right_bottom);

	_hoveredEdgePoint = nullptr;
	_clickedEdgePoint = nullptr;
}

void Canvas::setPosition(sf::Vector2f position) {
	_position = position;
	_bg_sprite.setPosition(_position);

	_point_left_top->setPosition(_position);
	_point_top->setPosition(_position + sf::Vector2f(getZoomedSize(_size).x / 2.0f, 0));
	_point_right_top->setPosition(_position + sf::Vector2f(getZoomedSize(_size).x, 0));
	_point_left->setPosition(_position + sf::Vector2f(0, getZoomedSize(_size).y / 2.0f));
	_point_right->setPosition(_position + sf::Vector2f(getZoomedSize(_size).x, getZoomedSize(_size).y / 2.0f));
	_point_left_bottom->setPosition(_position + sf::Vector2f(0, getZoomedSize(_size).y));
	_point_bottom->setPosition(_position + sf::Vector2f(getZoomedSize(_size).x / 2.0f, getZoomedSize(_size).y));
	_point_right_bottom->setPosition(_position + sf::Vector2f(getZoomedSize(_size).x, getZoomedSize(_size).y));

}


void Canvas::setZoom(float mouseWheelScrolllDelta) {

	sf::Vector2f mouseBeforeZoom = worldMousePosition - _position;
	float oldZoom = _zoom;

	_zoom += 0.25f * mouseWheelScrolllDelta;
	_zoom = std::clamp(_zoom, _min_zoom, _max_zoom);

	generateBackground(_size);

	sf::Vector2f mouseAfterZoom = mouseBeforeZoom * (_zoom / oldZoom);
	_position += (worldMousePosition - (_position + mouseAfterZoom));
	setPosition(_position);
}


void Canvas::drawPixels(sf::Color color) {

	std::vector<std::vector<bool>> b = brush->getBrush();
	sf::Image& image = animation->getCurrentLayer()->_image;
	for (int y = 0; y < b.size(); y++) {
		for (int x = 0; x < b[y].size(); x++) {
			if (b[y][x]) {

				int tx = brush->_pos_px.x - b[y].size() / 2 + x;
				int ty = brush->_pos_px.y - b.size() / 2 + y;

				if (tx < 0 || ty < 0 || tx >= _size.x || ty >= _size.y)
					continue;

				image.setPixel(tx, ty, color);
			}
		}
	}
}

void Canvas::fill(sf::Color colorToEdit, sf::Color newColor, sf::Vector2i pixelCoords) {

	if (colorToEdit == newColor)
		return;

	sf::IntRect imageRect = sf::IntRect(0, 0, animation->getCurrentLayer()->_image.getSize().x, animation->getCurrentLayer()->_image.getSize().y);
	sf::Image& image = animation->getCurrentLayer()->_image;

	std::vector < sf::Vector2i > pixels;
	pixels.push_back(pixelCoords);

	while (!pixels.empty()) {

		sf::Vector2i t = pixels.back();
		pixels.pop_back();

		if (!imageRect.contains(t))
			continue;

		if (image.getPixel(t.x, t.y) == colorToEdit) {
			image.setPixel(t.x, t.y, newColor);

			pixels.push_back(sf::Vector2i(t.x - 1, t.y));
			pixels.push_back(sf::Vector2i(t.x + 1, t.y));;
			pixels.push_back(sf::Vector2i(t.x, t.y - 1));;
			pixels.push_back(sf::Vector2i(t.x, t.y + 1));;
		}

	}

}

void Canvas::fillPixels(sf::Color color) {
	sf::Vector2i tile = worldToTile(worldMousePosition, _position, _zoom, _zoom_delta);
	sf::Color colorToEdit = animation->getCurrentLayer()->_image.getPixel(tile.x, tile.y);
	fill(colorToEdit, color, tile);
}

void Canvas::cursorHover() {

	if (!dialogs.empty())
		return;

	_hoveredEdgePoint = nullptr;

	for (auto& point : _edgePoints) {
		if (point->getGlobalBounds().contains(worldMousePosition)) {
			_hoveredEdgePoint = point;
			return;
		}
	}

	if (_bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
		ElementGUI_hovered = this;
	}

}

void Canvas::handleEvent(sf::Event& event) {

	_brush_is_visible = false;

	if (!dialogs.empty())
		return;

	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
		if (_hoveredEdgePoint != nullptr) {
			_clickedEdgePoint = _hoveredEdgePoint;
			_orginalEdgePointPosition = _point_left_top->getPosition();

			_state = CanvasState::Resizing;

			for (auto* f : _backupFrames) {
				for (auto* l : f->getLayers())
					delete l;
				delete f;
			}

			_backupFrames.clear();
			for (auto* frame : animation->getFrames()) {
				Frame* newFrame = new Frame();
				newFrame->_layers.reserve(frame->getLayers().size());

				for (auto* layer : frame->getLayers()) {
					Layer* newLayer = new Layer(layer->_name, sf::Vector2i(layer->_image.getSize()));
					newLayer->_image = layer->_image;
					newFrame->_layers.push_back(newLayer);
				}
				_backupFrames.push_back(newFrame);
			}

			return;
		}


	}
	else if (_state == CanvasState::Resizing) {
		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			_clickedEdgePoint = nullptr;
			_state = CanvasState::Idle;

			for (auto* f : _backupFrames) {
				for (auto* l : f->getLayers())
					delete l;
				delete f;
			}

			_backupFrames.clear();
		}
		else if (event.type == sf::Event::MouseMoved) {

		}

		return;
	}

	if (ElementGUI_hovered == this || ElementGUI_hovered == nullptr) {
		if (_bg_sprite.getGlobalBounds().contains(worldMousePosition)) {

			if (_state == CanvasState::Idle && _hoveredEdgePoint == nullptr) {
				if (toolbar->_toolType == ToolType::Brush || toolbar->_toolType == ToolType::Eraser) {
					_brush_is_visible = true;
					brush->setPosition(worldToTile(worldMousePosition, _position, _size, _zoom, _zoom_delta));
				}
			}


			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

				if (ElementGUI_pressed == this || ElementGUI_pressed == nullptr) {
					if (toolbar->_toolType == ToolType::Brush) {
						drawPixels(toolbar->_first_color->_color);
					}
					else if (toolbar->_toolType == ToolType::Eraser) {
						drawPixels(toolbar->_second_color->_color);
					}
					else if (toolbar->_toolType == ToolType::Fill) {
						fillPixels(toolbar->_first_color->_color);
					}
					else if (toolbar->_toolType == ToolType::Selector) {

						if (selection->_state == SelectionState::None) {
							sf::Vector2i tile = worldToTile(worldMousePosition, _position, _size, _zoom, _zoom_delta);
							selection->_state = SelectionState::Selecting;
							selection->_rect = sf::IntRect(tile.x, tile.y, 0, 0);
						}
					}
					else if (toolbar->_toolType == ToolType::Lasso) {

						if (lasso->_state == LassoState::None) {
							sf::Vector2i tile = worldToTile(worldMousePosition, _position, _size, _zoom, _zoom_delta);
							lasso->_state = LassoState::Selecting;
							lasso->_image = new sf::Image();
							lasso->_outlineOffset = tile;
							lasso->addPoint(tile);
						}
					}
				}
			}

			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {

			}

			else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

				if (_state == CanvasState::Idle) {
					if (ElementGUI_pressed == this || ElementGUI_pressed == nullptr) {
						if (toolbar->_toolType == ToolType::Brush) {
							drawPixels(toolbar->_first_color->_color);
						}
						else if (toolbar->_toolType == ToolType::Eraser) {
							drawPixels(toolbar->_second_color->_color);
						}
					}
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
				if (_state == CanvasState::Idle) {

				}
			}
			else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
				_state = CanvasState::Moving;
				_offset = _bg_sprite.getPosition() - worldMousePosition;
			}

			else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {

				if (_state == CanvasState::Idle) {

				}
			}
			else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle) {
				_state = CanvasState::Idle;
			}

			else if (event.type == sf::Event::MouseWheelScrolled) {
				setZoom(event.mouseWheelScroll.delta);
			}
		}
		else {
			_brush_is_visible = false;
		}
	}
	else {
		_brush_is_visible = false;
	}

	if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
		if (toolbar->_toolType == ToolType::Selector) {
			if (selection->_state == SelectionState::Moving) {
				sf::Vector2i tile = selectionToTile(worldMousePosition, _position, _size, selection->_rect.getSize(), selection->_offset, _zoom, _zoom_delta);
				sf::Vector2i dst = tile - selection->_offset;
				selection->_rect.left = dst.x;
				selection->_rect.top = dst.y;
			}
			else if (selection->_state == SelectionState::Selecting) {
				sf::Vector2i tile = worldToTile(worldMousePosition, _position, _size, _zoom, _zoom_delta);
				selection->_rect.width = tile.x - selection->_rect.left;
				selection->_rect.height = tile.y - selection->_rect.top;
			}

		}
		else if (toolbar->_toolType == ToolType::Lasso) {
			if (lasso->_state == LassoState::Moving) {
				sf::Vector2i tile = selectionToTile(worldMousePosition, _position, _size, lasso->_rect.getSize(), lasso->_offset, _zoom, _zoom_delta);
				sf::Vector2i dst = tile - lasso->_offset;
				lasso->_outlineOffset.x = dst.x;
				lasso->_outlineOffset.y = dst.y;
				lasso->generateRect();
			}
			else if (lasso->_state == LassoState::Selecting) {
				sf::Vector2i tile = worldToTile(worldMousePosition, _position, _size, _zoom, _zoom_delta);

				if (lasso->_image != nullptr) {
					lasso->generateRect();
					paste(&animation->getCurrentLayer()->_image, lasso->_image, lasso->_rect.left, lasso->_rect.top, toolbar->_second_color->_color);
					lasso->_image = nullptr;
				}

				lasso->addPoint(tile);
				lasso->generateRect();

				lasso->_image = new sf::Image();
				if (lasso->_rect.width > 0 && lasso->_rect.height > 0) {
					lasso->_image->create(lasso->_rect.width, lasso->_rect.height, sf::Color::Transparent);
					copy(lasso->_image, &animation->getCurrentLayer()->_image, lasso->_rect, toolbar->_second_color->_color);
					remove(animation->getCurrentLayer()->_image, lasso->_rect, lasso->generateMask(), toolbar->_second_color->_color);
				}
			}
		}
	}

	if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
		if (toolbar->_toolType == ToolType::Selector) {
			if (selection->_state == SelectionState::Selecting) {
				sf::IntRect norm = selection->normalizeRect();
				if (norm.width <= 0 || norm.height <= 0) {
					selection->_state = SelectionState::None;
				}
				else {
					selection->_rect = norm;
					selection->_state = SelectionState::Selected;
				}
			}
			else if (selection->_state == SelectionState::Moving) {
				selection->_rect = selection->normalizeRect();
				selection->_state = SelectionState::Selected;
			}
		}
		else if (toolbar->_toolType == ToolType::Lasso) {
			if (lasso->_state == LassoState::Selecting) {

				lasso->_state = LassoState::Selected;
			}
			else if (lasso->_state == LassoState::Moving) {
				lasso->_state = LassoState::Selected;
			}
		}
	}

	if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

		if (ElementGUI_pressed == nullptr) {
			if (toolbar->_toolType == ToolType::Selector) {
				sf::Vector2i tile = worldToTile(worldMousePosition, _position, _zoom, _zoom_delta);
				sf::IntRect norm = selection->normalizeRect();

				if (selection->clickOnSelection(tile)) {
					selection->_offset = tile - sf::Vector2i(norm.left, norm.top);
					selection->_state = SelectionState::Moving;

					if (selection->_img == nullptr) {
						selection->_img = new sf::Image();
						selection->_img->create(norm.width, norm.height, sf::Color::Transparent);
						copy(selection->_img, &animation->getCurrentLayer()->_image, lasso->_rect, toolbar->_second_color->_color);
						remove(animation->getCurrentLayer()->_image, norm, toolbar->_second_color->_color);
					}
				}
				else if (_bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
					if (selection->_img != nullptr) {
						paste(&animation->getCurrentLayer()->_image, selection->_img, norm.left, norm.top, toolbar->_second_color->_color);
						selection->_img = nullptr;
					}

					selection->_state = SelectionState::Selecting;
					selection->_rect = sf::IntRect(tile.x, tile.y, 0, 0);
				}
				else {
					if (selection->_img != nullptr) {
						paste(&animation->getCurrentLayer()->_image, selection->_img, norm.left, norm.top, toolbar->_second_color->_color);
						selection->_img = nullptr;
					}

					selection->_state = SelectionState::None;
					selection->_rect = sf::IntRect(0, 0, 0, 0);
				}
			}
			else if (toolbar->_toolType == ToolType::Lasso) {

				sf::Vector2i tile = worldToTile(worldMousePosition, _position, _zoom, _zoom_delta);

				if (lasso->clickOnSelection(tile)) {

					lasso->_state = LassoState::Moving;
					lasso->_offset = tile - lasso->_outlineOffset;

					if (lasso->_image == nullptr) {
						copy(&animation->getCurrentLayer()->_image, lasso->_image, lasso->_rect, toolbar->_second_color->_color);
						remove(animation->getCurrentLayer()->_image, lasso->_rect, toolbar->_second_color->_color);
					}
				}
				else if (_bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
					if (lasso->_image != nullptr) {
						paste(&animation->getCurrentLayer()->_image, lasso->_image, lasso->_rect.left, lasso->_rect.top, toolbar->_second_color->_color);
						lasso->_image = nullptr;

					}


					lasso->_state = LassoState::Selecting;
					lasso->unselect();
					lasso->_outlineOffset = tile;
					lasso->addPoint(tile);
				}
				else {
					if (lasso->_image != nullptr) {
						lasso->generateRect();
						paste(&animation->getCurrentLayer()->_image, lasso->_image, lasso->_rect.left, lasso->_rect.top, toolbar->_second_color->_color);
						lasso->_image = nullptr;
					}

					lasso->_state = LassoState::None;
					lasso->unselect();
					lasso->addPoint(tile);
				}

			}
		}

	}
}

float Canvas::clampAxisOverscroll(float v, float content, float viewport, float overscrollRatio) {
	float overscroll = viewport * overscrollRatio;
	float minPos = viewport - content - overscroll;
	float maxPos = overscroll;
	return std::clamp(v, minPos, maxPos);
}

void Canvas::update() {

	if (_state == CanvasState::Resizing) {
		sf::Vector2f p;
		p = (worldMousePosition + _edgePointSize / 2.0f - _clickedEdgePoint->getPosition()) / (_zoom * _zoom_delta);
		sf::Vector2i pp = sf::Vector2i(p.x, p.y);
		//std::cout << pp.x << ", " << pp.y << "\n";

		float minX, minY, maxX, maxY;

		minX = _point_left->getPosition().x;
		minY = _point_top->getPosition().y;
		maxX = _point_right->getPosition().x;
		maxY = _point_bottom->getPosition().y;

		if (_clickedEdgePoint == _point_left_top) {
			minX = _point_left->getPosition().x + float(pp.x) * _zoom * _zoom_delta;
			minY = _point_top->getPosition().y + float(pp.y) * _zoom * _zoom_delta;
		}
		else if (_clickedEdgePoint == _point_right_top) {
			minY = _point_top->getPosition().y + float(pp.y) * _zoom * _zoom_delta;
			maxX = _point_right->getPosition().x + float(pp.x) * _zoom * _zoom_delta;
		}
		else if (_clickedEdgePoint == _point_left_bottom) {
			minX = _point_left->getPosition().x + float(pp.x) * _zoom * _zoom_delta;
			maxY = _point_bottom->getPosition().y + float(pp.y) * _zoom * _zoom_delta;
		}
		else if (_clickedEdgePoint == _point_right_bottom) {
			maxX = _point_right->getPosition().x + float(pp.x) * _zoom * _zoom_delta;
			maxY = _point_bottom->getPosition().y + float(pp.y) * _zoom * _zoom_delta;
		}
		else if (_clickedEdgePoint == _point_top) {
			minY = _point_top->getPosition().y + float(pp.y) * _zoom * _zoom_delta;
		}
		else if (_clickedEdgePoint == _point_bottom) {
			maxY = _point_bottom->getPosition().y + float(pp.y) * _zoom * _zoom_delta;
		}
		else if (_clickedEdgePoint == _point_left) {
			minX = _point_left->getPosition().x + float(pp.x) * _zoom * _zoom_delta;
		}
		else if (_clickedEdgePoint == _point_right) {
			maxX = _point_right->getPosition().x + float(pp.x) * _zoom * _zoom_delta;
		}

		float minWpx = 16.0f * _zoom * _zoom_delta;
		float minHpx = 16.0f * _zoom * _zoom_delta;

		float maxWpx = 128.0f * _zoom * _zoom_delta;
		float maxHpx = 128.0f * _zoom * _zoom_delta;

		// Który bok/narożnik jest przeciągany?
		const bool movingLeft = (_clickedEdgePoint == _point_left || _clickedEdgePoint == _point_left_top || _clickedEdgePoint == _point_left_bottom);
		const bool movingRight = (_clickedEdgePoint == _point_right || _clickedEdgePoint == _point_right_top || _clickedEdgePoint == _point_right_bottom);
		const bool movingTop = (_clickedEdgePoint == _point_top || _clickedEdgePoint == _point_left_top || _clickedEdgePoint == _point_right_top);
		const bool movingBottom = (_clickedEdgePoint == _point_bottom || _clickedEdgePoint == _point_left_bottom || _clickedEdgePoint == _point_right_bottom);

		if ((maxX - minX) < minWpx) {
			if (movingLeft)  minX = maxX - minWpx;   // ciągniesz lewą krawędź → przytnij ją
			else             maxX = minX + minWpx;   // w innym wypadku trzymaj prawą
		}

		if ((maxY - minY) < minHpx) {
			if (movingTop)   minY = maxY - minHpx;   // ciągniesz górną krawędź → przytnij ją
			else             maxY = minY + minHpx;   // w innym wypadku trzymaj dolną
		}

		if ((maxX - minX) > maxWpx) {
			if (movingLeft)  minX = maxX - maxWpx;   // ciągniesz lewą krawędź → przytnij ją
			else             maxX = minX + maxWpx;   // w innym wypadku trzymaj prawą
		}

		if ((maxY - minY) > maxHpx) {
			if (movingTop)   minY = maxY - maxHpx;   // ciągniesz górną krawędź → przytnij ją
			else             maxY = minY + maxHpx;   // w innym wypadku trzymaj dolną
		}

		sf::Vector2i dst;
		dst.x = (_orginalEdgePointPosition.x - minX) / (_zoom * _zoom_delta);
		dst.y = (_orginalEdgePointPosition.y - minY) / (_zoom * _zoom_delta);



		_position = sf::Vector2f(minX, minY);

		_point_left_top->setPosition(minX, minY);
		_point_top->setPosition((minX + maxX) / 2, minY);
		_point_right_top->setPosition(maxX, minY);

		_point_left->setPosition(minX, (minY + maxY) / 2);
		_point_right->setPosition(maxX, (minY + maxY) / 2);

		_point_left_bottom->setPosition(minX, maxY);
		_point_bottom->setPosition((minX + maxX) / 2, maxY);
		_point_right_bottom->setPosition(maxX, maxY);

		////////////////////

		sf::Vector2i newSize;
		newSize.x = (maxX - minX) / (_zoom * _zoom_delta);
		newSize.y = (maxY - minY) / (_zoom * _zoom_delta);
		_size = newSize;

		generateBackground(_size);
		_bg_sprite.setPosition(_point_left_top->getPosition());

		if (p.x < 0)
			_offset.x = _offset.x - float(p.x) * _zoom * _zoom_delta;

		if (p.y < 0)
			_offset.y = _offset.y - float(p.y) * _zoom * _zoom_delta;

		const size_t framesCount = std::min(_backupFrames.size(), animation->getFrames().size());
		for (size_t f = 0; f < framesCount; ++f) {
			Frame* src = _backupFrames[f];
			Frame* org = animation->getFrames()[f];

			const size_t layersCount = std::min(src->getLayers().size(), org->getLayers().size());
			for (size_t l = 0; l < layersCount; ++l) {
				Layer* srcLayer = src->getLayers()[l];
				Layer* orgLayer = org->getLayers()[l];

				sf::Image newImage;
				newImage.create(_size.x, _size.y, toolbar->_second_color->_color);
				paste(&newImage, &srcLayer->_image, dst.x, dst.y);
				orgLayer->_image = newImage;
			}

		}
	}
	else if (_state == CanvasState::Moving) {
		sf::Vector2f target = worldMousePosition + _offset;
		float x = clampAxisOverscroll(target.x, _bg_sprite.getGlobalBounds().width, window->getSize().x, 0.5f);
		float y = clampAxisOverscroll(target.y, _bg_sprite.getGlobalBounds().height, window->getSize().y, 0.5f);
		setPosition(sf::Vector2f(x, y));
	}
}

void Canvas::draw() {

	window->draw(_bg_sprite);


	for (int i = 0; i < animation->getLayersCount(); i++) {

		if (layers_dialog->layersBoxes[i]->_visibling->_value == 0) {	// 0 - visible, 1 - invisible
			sf::Texture tex;
			tex.loadFromImage(animation->getLayer(i)->_image);

			sf::Sprite spr(tex);
			spr.setPosition(_bg_sprite.getPosition());
			spr.setScale(_zoom * _zoom_delta, _zoom * _zoom_delta);
			window->draw(spr);
		}
	}

	for (auto& point : _edgePoints) {
		window->draw(*point);
	}

	selection->draw(_bg_sprite.getPosition(), _size, _zoom * _zoom_delta);
	lasso->draw(_bg_sprite.getPosition(), _size, _zoom * _zoom_delta, toolbar->_second_color->_color);

	if (_brush_is_visible == true) {
		brush->draw(_position, _size, _zoom, _zoom_delta);
	}

}


Canvas* canvas = nullptr;