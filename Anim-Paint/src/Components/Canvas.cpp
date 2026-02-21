#include "Components/Canvas.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Cursor.hpp"
#include "Tools/Brush.hpp"
#include "Animation/Animation.hpp"
#include "WorldToTileConverter.hpp"
#include "Components/Toolbar.hpp"
#include "Tools/Selection.hpp"
#include "Tools/Lasso.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Dialogs/Dialog.hpp"
#include "DebugLog.hpp"
#include "Components/MainMenu.hpp"
#include "../../include/Dialogs/Palette.hpp"
#include "Tools/Line.hpp"
#include "History.hpp"
#include "Components/BottomBar.hpp"

Canvas::Canvas() : ElementGUI() {
	_minSize = sf::Vector2i(16, 16);
	_maxSize = sf::Vector2i(256, 256);
	_size = sf::Vector2i(64, 64);
	_pixelSize = 8;

	int targetSize = 32;
	int sides = std::max(_size.x, _size.y);
	
	_zoom_delta = 16.0f;
	_min_zoom = 0.125f;
	_max_zoom = 1.0f;
	_zoom = (float)(targetSize) / (float)(sides * 4);

	_size = sf::Vector2i(targetSize, targetSize);  

	_state = CanvasState::Idle;

	_offset = sf::Vector2i(0, 0);

	resize(_size);

	setPosition((sf::Vector2i(window->getSize()) - sf::Vector2i(getZoomedSize(_size))) / 2);

	_isEdited = false;
}

Canvas::~Canvas() {

}

void Canvas::resize(sf::Vector2i size) {

	_size = size;
	generateBackground(size);
	generateEdgePoints();
	setPosition(_position);
}

sf::Vector2i Canvas::getZoomedSize(sf::Vector2i size) {
	return sf::Vector2i(int((float)(size.x) * _zoom_delta * _zoom), int((float)(size.y) * _zoom_delta * _zoom));
}      

void Canvas::generateBackground(sf::Vector2i size) {

	_bg_image = sf::Image();
	sf::Vector2i s = getZoomedSize(size);
	_bg_image.resize(sf::Vector2u(s), sf::Color::Transparent);

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

			_bg_image.setPixel(sf::Vector2u(x,y), c); // tu jest błąd - wychodzi poza zakres

		}
	}

	_bg_texture = sf::Texture();
	if (!_bg_texture.loadFromImage(_bg_image)) {
		DebugError(L"Canvas::generateBackground: Failed to load texture from image.");
		exit(0);
	}

	_rect = sf::IntRect(_rect.position, s);
	
}

void Canvas::generateEdgePoints() {
	_edgePoints.clear();
	_point_left_top = std::make_shared<EdgePoint>(_position);
	_point_top = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x / 2, 0));
	_point_right_top = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x, 0));
	_point_left = std::make_shared<EdgePoint>(_position + sf::Vector2i(0, getZoomedSize(_size).y / 2));
	_point_right = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x, getZoomedSize(_size).y / 2));
	_point_left_bottom = std::make_shared<EdgePoint>(_position + sf::Vector2i(0, getZoomedSize(_size).y));
	_point_bottom = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x / 2, getZoomedSize(_size).y));
	_point_right_bottom = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x, getZoomedSize(_size).y));

	_edgePoints.push_back(_point_left_top);
	_edgePoints.push_back(_point_top);
	_edgePoints.push_back(_point_right_top);
	_edgePoints.push_back(_point_left);
	_edgePoints.push_back(_point_right);
	_edgePoints.push_back(_point_left_bottom);
	_edgePoints.push_back(_point_bottom);
	_edgePoints.push_back(_point_right_bottom);

	_hoveredEdgePoint = nullptr;
	_clickedEdgePoint = nullptr;
}

void Canvas::setPosition(sf::Vector2i position) {
	_position = position;
	
	_rect.position = position;

	_point_left_top->setPosition(_position);
	_point_top->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x / 2, 0));
	_point_right_top->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x, 0));
	_point_left->setPosition(_position + sf::Vector2i(0, getZoomedSize(_size).y / 2));
	_point_right->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x, getZoomedSize(_size).y / 2));
	_point_left_bottom->setPosition(_position + sf::Vector2i(0, getZoomedSize(_size).y));
	_point_bottom->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x / 2, getZoomedSize(_size).y));
	_point_right_bottom->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x, getZoomedSize(_size).y));

	if ((toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso) && selection->_state == SelectionState::Selected) {
		selection->generateEdgePoints();
	}
}

void Canvas::setCenter() {
	setPosition((sf::Vector2i(window->getSize()) - sf::Vector2i(getZoomedSize(_size))) / 2);
}


void Canvas::setZoom(float mouseWheelScrolllDelta) {

	sf::Vector2i mouseBeforeZoom = cursor->_worldMousePosition - _position;
	float oldZoom = _zoom;

	_zoom += 0.0625f * mouseWheelScrolllDelta;
	_zoom = std::clamp(_zoom, _min_zoom, _max_zoom);

	generateBackground(_size);

	sf::Vector2i mouseAfterZoom = sf::Vector2i(sf::Vector2f(mouseBeforeZoom) * (_zoom / oldZoom));
	_position += (cursor->_worldMousePosition - (_position + mouseAfterZoom));
	setPosition(_position);
}

void Canvas::resize() {
	sf::Vector2f p;
	p = (sf::Vector2f(cursor->_worldMousePosition) + sf::Vector2f(_edgePoints[0]->getSize()) / 2.0f - sf::Vector2f(_clickedEdgePoint->getPosition())) / (_zoom * _zoom_delta);
	sf::Vector2i pp = sf::Vector2i(p);
	//std::cout << pp.x << ", " << pp.y << "\n";

	float minX, minY, maxX, maxY;

	minX = (float)(_point_left->getPosition().x);
	minY = (float)(_point_top->getPosition().y);
	maxX = (float)(_point_right->getPosition().x);
	maxY = (float)(_point_bottom->getPosition().y);

	if (_clickedEdgePoint == _point_left_top) {
		minX = (float)(_point_left->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
		minY = (float)(_point_top->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (_clickedEdgePoint == _point_right_top) {
		minY = (float)(_point_top->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
		maxX = (float)(_point_right->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
	}
	else if (_clickedEdgePoint == _point_left_bottom) {
		minX = (float)(_point_left->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
		maxY = (float)(_point_bottom->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (_clickedEdgePoint == _point_right_bottom) {
		maxX = (float)(_point_right->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
		maxY = (float)(_point_bottom->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (_clickedEdgePoint == _point_top) {
		minY = (float)(_point_top->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (_clickedEdgePoint == _point_bottom) {
		maxY = (float)(_point_bottom->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (_clickedEdgePoint == _point_left) {
		minX = (float)(_point_left->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
	}
	else if (_clickedEdgePoint == _point_right) {
		maxX = (float)(_point_right->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
	}

	float minWpx = (float)(canvas->_minSize.x * _zoom * _zoom_delta);
	float minHpx = (float)(canvas->_minSize.y * _zoom * _zoom_delta);

	float maxWpx = (float)(canvas->_maxSize.x * _zoom * _zoom_delta);
	float maxHpx = (float)(canvas->_maxSize.y * _zoom * _zoom_delta);

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
	dst.x = (int)(((float)(_orginalEdgePointPosition.x) - minX) / (_zoom * _zoom_delta));
	dst.y = (int)(((float)(_orginalEdgePointPosition.y) - minY) / (_zoom * _zoom_delta));

	int iminX = (int)minX;
	int iminY = (int)minY;
	int imaxX = (int)maxX;
	int imaxY = (int)maxY;

	_position = sf::Vector2i(iminX, iminY);

	_point_left_top->setPosition(sf::Vector2i(iminX, iminY));
	_point_top->setPosition(sf::Vector2i((iminX + imaxX) / 2, iminY));
	_point_right_top->setPosition(sf::Vector2i(imaxX, iminY));

	_point_left->setPosition(sf::Vector2i(iminX, (iminY + imaxY) / 2));
	_point_right->setPosition(sf::Vector2i(imaxX, (iminY + imaxY) / 2));

	_point_left_bottom->setPosition(sf::Vector2i(iminX, imaxY));
	_point_bottom->setPosition(sf::Vector2i((iminX + imaxX) / 2, imaxY));
	_point_right_bottom->setPosition(sf::Vector2i(imaxX, imaxY));

	////////////////////

	sf::Vector2i newSize;
	newSize.x = (int)((maxX - minX) / (_zoom * _zoom_delta));
	newSize.y = (int)((maxY - minY) / (_zoom * _zoom_delta));
	_size = newSize;

	generateBackground(_size);

	_rect.position = _point_left_top->getPosition();

	if (p.x < 0)
		_offset.x = _offset.x - (int)(float(p.x) * _zoom * _zoom_delta);

	if (p.y < 0)
		_offset.y = _offset.y - (int)(float(p.y) * _zoom * _zoom_delta);

	const size_t framesCount = std::min(_backupFrames.size(), getCurrentAnimation()->getFrames().size());

	for (size_t f = 0; f < framesCount; ++f) {
		std::shared_ptr<Frame> src = _backupFrames[f];
		std::shared_ptr<Frame> org = getCurrentAnimation()->getFrames()[f];

		const size_t layersCount = std::min(src->getLayers().size(), org->getLayers().size());
		for (size_t l = 0; l < layersCount; ++l) {
			std::shared_ptr<Layer> srcLayer = src->getLayers()[l];
			std::shared_ptr<Layer> orgLayer = org->getLayers()[l];

			sf::Image newImage;
			newImage.resize(sf::Vector2u(_size), toolbar->_second_color->_color);
			pasteImageWithAlpha(newImage, srcLayer->_image, dst.x, dst.y);
			orgLayer->_image = newImage;
		}

	}
}

void Canvas::drawPixels(sf::Color color) {

	std::shared_ptr<Layer> layer = getCurrentAnimation()->getCurrentLayer();

	if (layer == nullptr)
		return;

	std::vector<sf::Vector2i> pixelsToDraw = getPointsFromLine(brush->_lastPosition, brush->_position);

	std::vector<std::vector<bool>> b = brush->getBrush();
	sf::Image& image = layer->_image;

	for (auto& pixel : pixelsToDraw) {
		for (int y = 0; y < b.size(); y++) {
			for (int x = 0; x < b[y].size(); x++) {
				if (b[y][x]) {

					int tx = pixel.x - (int)(b[y].size()) / 2 + x;
					int ty = pixel.y - (int)(b.size()) / 2 + y;

					if (tx < 0 || ty < 0 || tx >= _size.x || ty >= _size.y)
						continue;

					image.setPixel(sf::Vector2u(tx, ty), color);
				}
			}
		}
	}

}

void Canvas::fill(sf::Color colorToEdit, sf::Color newColor, sf::Vector2i pixelCoords) {

	if (colorToEdit == newColor)
		return;

	sf::IntRect imageRect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(getCurrentAnimation()->getCurrentLayer()->_image.getSize()));
	sf::Image& image = getCurrentAnimation()->getCurrentLayer()->_image;

	std::vector < sf::Vector2i > pixels;
	pixels.push_back(pixelCoords);

	while (!pixels.empty()) {

		sf::Vector2i t = pixels.back();
		pixels.pop_back();

		if (!imageRect.contains(t))
			continue;

		if (image.getPixel(sf::Vector2u(t.x, t.y)) == colorToEdit) {
			image.setPixel(sf::Vector2u(t.x, t.y), newColor);

			pixels.push_back(sf::Vector2i(t.x - 1, t.y));
			pixels.push_back(sf::Vector2i(t.x + 1, t.y));;
			pixels.push_back(sf::Vector2i(t.x, t.y - 1));;
			pixels.push_back(sf::Vector2i(t.x, t.y + 1));;
		}

	}

}

void Canvas::fillPixels(sf::Color color) {
	sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _zoom, _zoom_delta);
	sf::IntRect imageRect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(getCurrentAnimation()->getCurrentLayer()->_image.getSize()));

	if (!imageRect.contains(tile))
		return;

	sf::Color colorToEdit = getCurrentAnimation()->getCurrentLayer()->_image.getPixel(sf::Vector2u(tile.x, tile.y));
	fill(colorToEdit, color, tile);
}

void Canvas::pickPixel() {
	sf::Image image = getCurrentAnimation()->getCurrentLayer()->_image;
	sf::IntRect imageRect(sf::Vector2i(0, 0), sf::Vector2i(image.getSize()));
	sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _rect.position, _zoom, _zoom_delta);
	
	if (!imageRect.contains(tile))
		return;

	sf::Color color = image.getPixel(sf::Vector2u(tile));

	if (palette != nullptr) {
		palette->setColorInRGBInputs(color);
		palette->loadColorFromRGBInputs();
	}

	toolbar->_selectedColorButton->_color = color;
	
}

void Canvas::mouseLeftButtonPressedEvent() {
	if (ElementGUI_pressed.get() == this || ElementGUI_pressed.get() == nullptr) {
		if (toolbar->_toolType == ToolType::Brush) {
			drawPixels(toolbar->_first_color->_color);
			_isEdited = true;
		}
		else if (toolbar->_toolType == ToolType::Eraser) {
			drawPixels(toolbar->_second_color->_color);
			_isEdited = true;
		}
		else if (toolbar->_toolType == ToolType::Fill) {
			fillPixels(toolbar->_first_color->_color);
			_isEdited = true;
		}
		else if (toolbar->_toolType == ToolType::Picker) {
			pickPixel();
			
		}
	}

	if (ElementGUI_pressed.get() == nullptr || ElementGUI_pressed.get() == this || selection->_state == SelectionState::Selected) {

		sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _zoom, _zoom_delta);

		if ((toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso) && selection->clickOnSelection(tile)) {
			selection->_state = SelectionState::Moving;
			selection->_offset = tile - selection->_resizedRect.position;
		}
		else if (toolbar->_toolType == ToolType::Lasso || toolbar->_toolType == ToolType::Selector) {

			if (toolbar->_btn_copy->_state == ButtonState::Idle && toolbar->_btn_cut->_state == ButtonState::Idle && toolbar->_btn_paste->_state == ButtonState::Idle) {
				if (_rect.contains(cursor->_worldMousePosition)) {
					if (selection->_rect.size.x > 1 && selection->_rect.size.y > 1) {
						copyImageWithMask(getCurrentAnimation()->getCurrentLayer()->_image, *selection->_resizedImage, selection->_resizedRect.position.x, selection->_resizedRect.position.y, 0, 0, *selection->_resizedMaskImage, toolbar->_second_color->_color);
						selection->_image = nullptr;
						selection->_resizedImage = nullptr;
						history->saveStep();
					}
					selection->_state = SelectionState::Selecting;
					selection->_points.clear();
					selection->generateRect();
					selection->_resizedRect = selection->_rect;
					selection->_outlineOffset = tile;
				}
				else {
					if (selection->_rect.size.x > 1 && selection->_rect.size.y > 1) {
						copyImageWithMask(getCurrentAnimation()->getCurrentLayer()->_image, *selection->_resizedImage, selection->_rect.position.x, selection->_rect.position.y, 0, 0, *selection->_resizedMaskImage, toolbar->_second_color->_color);
						selection->_image = nullptr;
						selection->_resizedImage = nullptr;
						history->saveStep();
					}

					selection->_state = SelectionState::None;
					selection->_points.clear();
					selection->_outlineOffset = sf::Vector2i(0, 0);
					selection->generateRect();
					selection->_resizedRect = selection->_rect;
				}
			}

		}
	}
}

void Canvas::mouseRightButtonPressedEvent() {
	if (ElementGUI_pressed.get() == this || ElementGUI_pressed.get() == nullptr) {
		if (toolbar->_toolType == ToolType::Brush) {
			drawPixels(toolbar->_second_color->_color);
			_isEdited = true;
		}
		else if (toolbar->_toolType == ToolType::Eraser) {
			drawPixels(toolbar->_first_color->_color);
			_isEdited = true;
		}
		else if (toolbar->_toolType == ToolType::Fill) {
			fillPixels(toolbar->_second_color->_color);
			_isEdited = true;
		}

	}
}

void Canvas::mouseLeftButtonReleasedEvent() {

	if (toolbar->_toolType == ToolType::Lasso || toolbar->_toolType == ToolType::Selector) {

		selection->generateRect();
		selection->generateMask();
		
		if (selection->_state == SelectionState::Selecting) {
			selection->_resizedRect = selection->_rect;
			selection->_resizedMaskImage = selection->_maskImage;
			selection->_resizedImage = selection->_image;
		}
		
		if (selection->_state == SelectionState::Selecting) {
			if (selection->_rect.size.x < 2 || selection->_rect.size.y < 2) {
				selection->_state = SelectionState::None;
				selection->_points.clear();
				selection->_outlineOffset = sf::Vector2i(0, 0);
			}
			else {
				if (selection->_rect.size.x > 1 && selection->_rect.size.y > 1) {
					copyImageWithMask(*selection->_image, getCurrentAnimation()->getCurrentLayer()->_image, 0, 0, selection->_rect.position.x, selection->_rect.position.y, *selection->_maskImage, toolbar->_second_color->_color);
					removeImageWithMask(getCurrentAnimation()->getCurrentLayer()->_image, selection->_rect, *selection->_maskImage, toolbar->_second_color->_color);
					selection->_resizedImage = selection->_image;
				}
				selection->_state = SelectionState::Selected;
				selection->generateEdgePoints();
			}

		}
		else if (selection->_state == SelectionState::Moving) {
			selection->_state = SelectionState::Selected;
			selection->generateEdgePoints();
		}else if (_isEdited) {
			history->saveStep();
			_isEdited = false;
		}
	}
}
void Canvas::mouseRightButtonReleasedEvent() {

	if (_isEdited) {
		history->saveStep();
		_isEdited = false;
	}
}


void Canvas::mouseMovedWithLeftButtonPressedEvent() {

	if (_state == CanvasState::Idle) {
		if (ElementGUI_pressed.get() == this || ElementGUI_pressed.get() == nullptr) {
			if (toolbar->_toolType == ToolType::Brush) {
				drawPixels(toolbar->_first_color->_color);
				return;
			}
			else if (toolbar->_toolType == ToolType::Eraser) {
				drawPixels(toolbar->_second_color->_color);
				return;
			}
			else if (toolbar->_toolType == ToolType::Picker) {
				pickPixel();
				return;
			}
		}
	}

	if ((toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso) && selection->_state == SelectionState::Moving) {
		sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _zoom, _zoom_delta);

		sf::Vector2i desiredRectPos = tile - selection->_offset;
		desiredRectPos.x = std::clamp(desiredRectPos.x, -selection->_resizedRect.size.x, _size.x);
		desiredRectPos.y = std::clamp(desiredRectPos.y, -selection->_resizedRect.size.y, _size.y);

		int minX = INT_MAX, minY = INT_MAX;
		for (auto& p : selection->_points) {
			minX = std::min(minX, p.x);
			minY = std::min(minY, p.y);
		}

		selection->_outlineOffset = desiredRectPos - sf::Vector2i(minX, minY);
		selection->generateRect();
		selection->_resizedRect.position = selection->_rect.position;
	}

	else if (toolbar->_toolType == ToolType::Selector) {
		if (selection->_state == SelectionState::Selecting) {

			sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _size, _zoom, _zoom_delta);

			if (selection->_image != nullptr) {
				selection->_image = nullptr;
				selection->_resizedImage = nullptr;
			}
				

			// LOCAL względem punktu kliknięcia
			sf::Vector2i local = tile - selection->_outlineOffset;

			int minX = std::min(0, local.x);
			int minY = std::min(0, local.y);
			int maxX = std::max(0, local.x);
			int maxY = std::max(0, local.y);

			selection->_points.clear();
			selection->_points.push_back(sf::Vector2i(minX, minY)); // LT
			selection->_points.push_back(sf::Vector2i(maxX, minY)); // RT
			selection->_points.push_back(sf::Vector2i(maxX, maxY)); // RB
			selection->_points.push_back(sf::Vector2i(minX, maxY)); // LB
			selection->_points.push_back(sf::Vector2i(minX, minY)); // close

			selection->generateRect();
			selection->_resizedRect = selection->_rect;
			selection->_image = std::make_shared<sf::Image>();
			selection->_image->resize(sf::Vector2u(1, 1), sf::Color::Transparent);
			if (selection->_rect.size.x > 1 && selection->_rect.size.y > 1) {
				selection->_image->resize(sf::Vector2u(selection->_rect.size), sf::Color::Transparent);
			}

		}
	}
	else if (toolbar->_toolType == ToolType::Lasso) {
		
		if (selection->_state == SelectionState::Selecting) {
			sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _size, _zoom, _zoom_delta);

			if (selection->_image != nullptr) {
				selection->_image = nullptr;
				selection->_resizedImage = nullptr;
			}

			selection->addPoint(tile);
			selection->generateRect();
			selection->_resizedRect = selection->_rect;

			selection->_image = std::make_shared<sf::Image>();
			selection->_image->resize(sf::Vector2u(1, 1), sf::Color::Transparent);
			if (selection->_rect.size.x > 1 && selection->_rect.size.y > 1) {
				selection->_image->resize(sf::Vector2u(selection->_rect.size), sf::Color::Transparent);
			}

		}
	}
}

void Canvas::mouseMovedWithRightButtonPressedEvent() {

	if (_state == CanvasState::Idle) {
		if (ElementGUI_pressed.get() == this || ElementGUI_pressed.get() == nullptr) {
			if (toolbar->_toolType == ToolType::Brush) {
				drawPixels(toolbar->_second_color->_color);
			}
			else if (toolbar->_toolType == ToolType::Eraser) {
				drawPixels(toolbar->_first_color->_color);
			}
		}
	}
}

void Canvas::cursorHover() {

	_hoveredEdgePoint = nullptr;

	if (!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (_rect.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}

	if (toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso) {
		for (auto& point : _edgePoints) {
			point->cursorHover();
			if (ElementGUI_hovered == point) {
				_hoveredEdgePoint = point;
			}
		}
	}

	if ((toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso) && selection->_state == SelectionState::Selected) {

		selection->_hoveredEdgePoint = nullptr;

		for (auto& point : selection->_edgePoints) {
			point->cursorHover();
			if (ElementGUI_hovered == point) {
				selection->_hoveredEdgePoint = point;
			}
		}
	}

	if(selection->_state == SelectionState::Selecting || selection->_state == SelectionState::Resizing) {
		ElementGUI_hovered = this->shared_from_this();
	}

}

void Canvas::handleEvent(const sf::Event& event) {


	if (!dialogs.empty()) {
		return;
	}

	if (main_menu->_state != MainMenuStates::Closed) {
		return;
	}

	if ((_clickedEdgePoint != nullptr && ElementGUI_hovered != _clickedEdgePoint) && ElementGUI_hovered.get() != nullptr && ElementGUI_hovered.get() != this && selection->_state != SelectionState::Selecting) {
		return;
	}

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (_rect.contains(cursor->_worldMousePosition)) {
			ElementGUI_pressed = this->shared_from_this();
		}
	}

	if (dialogs.empty() && toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso) {
		for (auto& point : _edgePoints) {
			point->handleEvent(event);
		}
	}

	if (dialogs.empty() && (toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso) && selection->_state == SelectionState::Selected) {
		for (auto& point : selection->_edgePoints) {
			point->handleEvent(event);
		}
	}
	
	// selection resizing
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (selection->_state == SelectionState::Selected && selection->_hoveredEdgePoint != nullptr && ElementGUI_hovered == selection->_hoveredEdgePoint) {
			selection->_clickedEdgePoint = selection->_hoveredEdgePoint;
			selection->_orginalEdgePointPosition = selection->_point_left_top->getPosition();
			selection->_state = SelectionState::Resizing;
		}
	}
	else if (selection->_state == SelectionState::Resizing) {
		if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			selection->_clickedEdgePoint = nullptr;
			selection->_state = SelectionState::Selected;
		}
		return;
	}

	// canvas resizing
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (dialogs.empty() && toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso) {
			if (_hoveredEdgePoint != nullptr && ElementGUI_hovered == _hoveredEdgePoint) {
				_clickedEdgePoint = _hoveredEdgePoint;
				_orginalEdgePointPosition = _point_left_top->getPosition();

				_state = CanvasState::Resizing;

				_backupFrames.clear();
				for (auto& frame : getCurrentAnimation()->getFrames()) {
					std::shared_ptr<Frame> newFrame = std::make_shared<Frame>();
					newFrame->_layers.reserve(frame->getLayers().size());

					for (auto& layer : frame->getLayers()) {
						std::shared_ptr<Layer> newLayer = std::make_shared<Layer>(layer->_name, sf::Vector2i(layer->_image.getSize()));
						newLayer->_image = layer->_image;
						newFrame->_layers.push_back(newLayer);
					}
					_backupFrames.push_back(newFrame);
				}
				return;
			}
		}
	}
	else if (_state == CanvasState::Resizing) {
		if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			_clickedEdgePoint = nullptr;
			_state = CanvasState::Idle;
			_backupFrames.clear();
		}
		return;
	}

	if (ElementGUI_hovered.get() == this || ElementGUI_hovered.get() == nullptr) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			mouseLeftButtonPressedEvent();
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			mouseRightButtonReleasedEvent();
		}
		else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Right) {
			mouseRightButtonPressedEvent();
		}
		else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Middle) {
			_state = CanvasState::Moving; 
			_offset = _rect.position - cursor->_worldMousePosition;
		}

		else if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
			mouseMovedWithRightButtonPressedEvent();
		}

		else if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>()) {
			setZoom(mws->delta);
		}
		else {

		}
	}


	if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv!=nullptr && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		mouseMovedWithLeftButtonPressedEvent();
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		mouseLeftButtonReleasedEvent();
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Middle) {
		if (_state == CanvasState::Moving) {
			_state = CanvasState::Idle;
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

	if (selection->_state == SelectionState::Resizing) {
		for (auto& point : selection->_edgePoints) {
			point->update();
		}
		selection->resizeRect();
		selection->generateResizedMask();
		selection->resizeImage();
		return;
	}

	for (auto& point : _edgePoints) {
		point->update();
	}

	if (_state == CanvasState::Resizing) {
		resize();
	}
	else if (_state == CanvasState::Moving) {
		sf::Vector2f target = sf::Vector2f(cursor->_worldMousePosition + _offset);
		int x = (int)(clampAxisOverscroll(target.x, (float)(_rect.size.x), (float)(window->getSize().x), 0.5f));
		int y = (int)(clampAxisOverscroll(target.y, (float)(_rect.size.y), (float)(window->getSize().y), 0.5f));
		setPosition(sf::Vector2i(x, y));
	}
	
}

void Canvas::draw() {

	sf::Sprite sprite(_bg_texture);
	sprite.setPosition(sf::Vector2f(_rect.position));
	window->draw(sprite);

	for (int i = 0; i < getCurrentAnimation()->getLayersCount(); i++) {

		if (layers_dialog->layersBoxes[i]->_visibling->_value == 0) {	// 0 - visible, 1 - invisible
			sf::Texture tex;
			if (!tex.loadFromImage(getCurrentAnimation()->getLayer(i)->_image)) {
				DebugError(L"Canvas::draw: Failed to load texture from image.");
				exit(0);
			}

			sf::Sprite spr(tex);
			spr.setPosition(sf::Vector2f(_rect.position));
			spr.setScale(sf::Vector2f(_zoom * _zoom_delta, _zoom * _zoom_delta));
			window->draw(spr);
		}
	}

	if (dialogs.empty() && toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso) {
		for (auto& point : _edgePoints) {
			point->draw();
		}
	}
	

	selection->draw(toolbar->_second_color->_color);

	

}


std::shared_ptr<Canvas> canvas;