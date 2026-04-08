#include "Components/Canvas.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Cursor.hpp"
#include "Tools/Brush.hpp"
#include "Animation/Animation.hpp"
#include "WorldToTileConverter.hpp"
#include "Components/Toolbar/Toolbar.hpp"
#include "Tools/Selection.hpp"
#include "Tools/ResizableTool.hpp"
#include "Components/LayersPanel/LayersPanel.hpp"
#include "Dialogs/Dialog.hpp"
#include "DebugLog.hpp"
#include "Components/MainMenu/MainMenu.hpp"
#include "Dialogs/Palette.hpp"
#include "Tools/Line.hpp"
#include "History.hpp"
#include "Components/BottomBar.hpp"
#include "Time.hpp"

Canvas::Canvas() : Element() {
	_minSize = sf::Vector2i(16, 16);
	_maxSize = sf::Vector2i(256, 256);
	_pixelSize = 8;

	reset();
	setCenter();
}

Canvas::~Canvas() {

}

void Canvas::reset() {
	int targetSize = 32;
	int sides = std::max(targetSize, targetSize);

	_zoom_delta = 16.0f;
	_min_zoom = 0.125f;
	_max_zoom = 1.0f;
	_zoom = (float)(targetSize) / (float)(sides * 4);

	_size = sf::Vector2i(targetSize, targetSize);
	_state = CanvasState::Idle;
	_offset = sf::Vector2i(0, 0);

	_isEdited = false;

	generateBackground(_size);
	generateEdgePoints();
}

sf::Vector2i Canvas::getZoomedSize(sf::Vector2i size) {
	return sf::Vector2i(int((float)(size.x) * _zoom_delta * _zoom), int((float)(size.y) * _zoom_delta * _zoom));
}      

void Canvas::setCenter() {
	setPosition((sf::Vector2i(window->getSize()) - sf::Vector2i(getZoomedSize(_size))) / 2);
}

void Canvas::resize(sf::Vector2i newSize) {
	_size = newSize;
	generateBackground(_size);
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

			_bg_image.setPixel(sf::Vector2u(x,y), c);

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

	if ((toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso) && selection->_state == ResizableToolState::Selected) {
		selection->generateEdgePoints();
	}

	if(resizable_tool && resizable_tool->_state == ResizableToolState::Selected) {
		resizable_tool->generateEdgePoints();
	}
}




void Canvas::setZoom(float mouseWheelScrolllDelta) {

	sf::Vector2i mouseBeforeZoom = cursor->_position - _position;
	float oldZoom = _zoom;

	_zoom += 0.0625f * mouseWheelScrolllDelta;
	_zoom = std::clamp(_zoom, _min_zoom, _max_zoom);

	generateBackground(_size);

	sf::Vector2i mouseAfterZoom = sf::Vector2i(sf::Vector2f(mouseBeforeZoom) * (_zoom / oldZoom));
	_position += (cursor->_position - (_position + mouseAfterZoom));
	setPosition(_position);
}

void Canvas::resize(std::shared_ptr<EdgePoint> edgePoint, sf::Vector2i cursorPosition) {

	if(edgePoint == nullptr)
		return;

	sf::Vector2f p;
	p = (sf::Vector2f(cursorPosition) - sf::Vector2f(edgePoint->getPosition())) / (_zoom * _zoom_delta);
	sf::Vector2i pp = sf::Vector2i(p);
	
	//std::wcout << pp.x << ", " << pp.y << "\n";

	float minX, minY, maxX, maxY;

	minX = (float)(_point_left->getPosition().x);
	minY = (float)(_point_top->getPosition().y);
	maxX = (float)(_point_right->getPosition().x);
	maxY = (float)(_point_bottom->getPosition().y);

	if (edgePoint == _point_left_top) {
		minX = (float)(_point_left->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
		minY = (float)(_point_top->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (edgePoint == _point_right_top) {
		minY = (float)(_point_top->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
		maxX = (float)(_point_right->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
	}
	else if (edgePoint == _point_left_bottom) {
		minX = (float)(_point_left->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
		maxY = (float)(_point_bottom->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (edgePoint == _point_right_bottom) {
		maxX = (float)(_point_right->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
		maxY = (float)(_point_bottom->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (edgePoint == _point_top) {
		minY = (float)(_point_top->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (edgePoint == _point_bottom) {
		maxY = (float)(_point_bottom->getPosition().y) + float(pp.y) * _zoom * _zoom_delta;
	}
	else if (edgePoint == _point_left) {
		minX = (float)(_point_left->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
	}
	else if (edgePoint == _point_right) {
		maxX = (float)(_point_right->getPosition().x) + float(pp.x) * _zoom * _zoom_delta;
	}

	float minWpx = (float)(canvas->_minSize.x * _zoom * _zoom_delta);
	float minHpx = (float)(canvas->_minSize.y * _zoom * _zoom_delta);

	float maxWpx = (float)(canvas->_maxSize.x * _zoom * _zoom_delta);
	float maxHpx = (float)(canvas->_maxSize.y * _zoom * _zoom_delta);

	// Który bok/narożnik jest przeciągany?
	const bool movingLeft = (edgePoint == _point_left || edgePoint == _point_left_top || edgePoint == _point_left_bottom);
	const bool movingRight = (edgePoint == _point_right || edgePoint == _point_right_top || edgePoint == _point_right_bottom);
	const bool movingTop = (edgePoint == _point_top || edgePoint == _point_left_top || edgePoint == _point_right_top);
	const bool movingBottom = (edgePoint == _point_bottom || edgePoint == _point_left_bottom || edgePoint == _point_right_bottom);

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
	dst.x = (int)(((float)(_topLeftPointPosition.x) - minX) / (_zoom * _zoom_delta));
	dst.y = (int)(((float)(_topLeftPointPosition.y) - minY) / (_zoom * _zoom_delta));

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
			newImage.resize(sf::Vector2u(_size), sf::Color::Transparent);
			pasteImageWithAlpha(newImage, srcLayer->_image, dst.x, dst.y);
			orgLayer->_image = newImage;
		}

	}
}

void Canvas::drawPixels(sf::Color color) {

	std::shared_ptr<Layer> layer = getCurrentAnimation()->getCurrentLayer();

	if (layer == nullptr) {
		return;
	}

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

	std::shared_ptr<Layer> layer = getCurrentAnimation()->getCurrentLayer();

	if (layer == nullptr) {
		return;
	}

	sf::Vector2i tile = worldToTile(cursor->_position, _position, _zoom, _zoom_delta);
	sf::IntRect imageRect = sf::IntRect(sf::Vector2i(0, 0), sf::Vector2i(getCurrentAnimation()->getCurrentLayer()->_image.getSize()));

	if (!imageRect.contains(tile))
		return;

	sf::Color colorToEdit = layer->_image.getPixel(sf::Vector2u(tile.x, tile.y));
	fill(colorToEdit, color, tile);
}

void Canvas::pickPixel() {

	std::shared_ptr<Layer> layer = getCurrentAnimation()->getCurrentLayer();
	if (layer == nullptr) {
		return;
	}

	sf::Image image = getCurrentAnimation()->getCurrentLayer()->_image;
	sf::IntRect imageRect(sf::Vector2i(0, 0), sf::Vector2i(image.getSize()));
	sf::Vector2i tile = worldToTile(cursor->_position, _rect.position, _zoom, _zoom_delta);
	
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

	if(_isEdited)
		_isEdited = false;

	if (Element_pressed.get() == this || (Element_pressed.get() == nullptr && Element_hovered.get() == this)) {
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
}

void Canvas::mouseRightButtonPressedEvent() {
	if (Element_pressed.get() == this || (Element_pressed.get() == nullptr && Element_hovered.get() == this)) {
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
	if (_isEdited) {
		history->saveStep();
		_isEdited = false;
	}
}

void Canvas::mouseRightButtonReleasedEvent() {

}


void Canvas::mouseMovedWithLeftButtonPressedEvent() {

	if (_state == CanvasState::Idle) {
		if (Element_pressed.get() == this || (Element_pressed.get() == nullptr && Element_hovered.get() == this)) {
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
}

void Canvas::mouseMovedWithRightButtonPressedEvent() {

	if (_state == CanvasState::Idle) {
		if (Element_pressed.get() == this || (Element_pressed.get() == nullptr && Element_hovered.get() == this)) {
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

	

	if (!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	if (!static_dialogs.empty() && static_dialogs.back()->_is_moved)
		return;

	if (resizable_tool != nullptr && (resizable_tool->_state == ResizableToolState::Moving || resizable_tool->_state == ResizableToolState::Selecting || resizable_tool->_state == ResizableToolState::Resizing))
		return;

	if (_rect.contains(cursor->_position)) {
		Element_hovered = this->shared_from_this();
	}

	_hoveredEdgePoint = nullptr;
	
	if (_state == CanvasState::Idle) {
		if (toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso && !(resizable_tool != nullptr && resizable_tool->tooltypeIsShape())) {
			for (auto& point : _edgePoints) {
				point->cursorHover();
				if (Element_hovered == point) {
					_hoveredEdgePoint = point;
				}
			}
		}
	}
	
	if ((toolbar->_toolType == ToolType::Selector || toolbar->_toolType == ToolType::Lasso || (resizable_tool!= nullptr && resizable_tool->tooltypeIsShape())) && resizable_tool->_state == ResizableToolState::Selected) {

		resizable_tool->_hoveredEdgePoint = nullptr;

		for (auto& point : resizable_tool->_edgePoints) {
			point->cursorHover();
			if (Element_hovered == point) {
				resizable_tool->_hoveredEdgePoint = point;
			}
		}
	}

	if(resizable_tool!=nullptr && (resizable_tool->_state == ResizableToolState::Selecting || resizable_tool->_state == ResizableToolState::Resizing)) {
		Element_hovered = this->shared_from_this();
	}

}

void Canvas::handleEvent(const sf::Event& event) {

	if(!dialogs.empty())
		return;

	if (main_menu->_state != MainMenuStates::Closed)
		return;

	if (toolbar->_btn_paste_menu->_isOpen)
		return;

	// canvas resizing
	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (dialogs.empty() && toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso) {
			if (_hoveredEdgePoint != nullptr && Element_hovered == _hoveredEdgePoint) {
				_clickedEdgePoint = _hoveredEdgePoint;
				Element_pressed = _clickedEdgePoint;
				_orginalEdgePointPosition = _clickedEdgePoint->getPosition();
				_topLeftPointPosition = _point_left_top->getPosition();
				_state = CanvasState::Resizing;

				_backupFrames.clear();
				for (auto& frame : getCurrentAnimation()->getFrames()) {
					std::shared_ptr<Frame> newFrame = std::make_shared<Frame>();
					newFrame->_layers.reserve(frame->getLayers().size());

					for (auto& layer : frame->getLayers()) {
						std::shared_ptr<Layer> newLayer = std::make_shared<Layer>(layer->_name, sf::Vector2i(layer->_image.getSize()), sf::Color::Transparent);
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
			if(Element_pressed == _clickedEdgePoint) {
				Element_pressed = nullptr;
			}
			history->saveStep();
			_clickedEdgePoint = nullptr;
			_state = CanvasState::Idle;
			_backupFrames.clear();
		}
		return;
	}

	if (!dialogs.empty()) {
		return;
	}

	if (main_menu->_state != MainMenuStates::Closed) {
		return;
	}

	if (!(Element_hovered.get() == this || Element_hovered == nullptr || (resizable_tool!= nullptr && Element_hovered == resizable_tool))) {
		return;
	}

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
		if (Element_hovered.get() == this && _rect.contains(cursor->_position)) {
			Element_pressed = this->shared_from_this();
		}
	}

	if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (Element_pressed.get() == this && !_rect.contains(cursor->_position)) {
			Element_pressed = nullptr;
		}
	}

	if (dialogs.empty() && toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso) {
		for (auto& point : _edgePoints) {
			point->handleEvent(event);
		}
	}

	if(Element_hovered.get() == this || Element_hovered.get() == nullptr || (resizable_tool!= nullptr && Element_hovered == resizable_tool)) {
		if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>()) {
			setZoom(mws->delta);
		}
		else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Middle) {
			if(_rect.contains(cursor->_position)) {
				_state = CanvasState::Moving;
				_offset = _rect.position - cursor->_position;
			}
		}
	}

	if (Element_hovered.get() == this || Element_hovered.get() == nullptr) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			mouseLeftButtonPressedEvent();
		}
		else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			mouseRightButtonReleasedEvent();
		}
		else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Right) {
			mouseRightButtonPressedEvent();
		}
		else if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {
			mouseMovedWithRightButtonPressedEvent();
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

	for (auto& point : _edgePoints) {
		point->update();
	}

	if (_state == CanvasState::Resizing) {
		resize(_clickedEdgePoint, cursor->_position);
	}
	else if (_state == CanvasState::Moving) {
		sf::Vector2f target = sf::Vector2f(cursor->_position + _offset);
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

		if (layers_panel->layersBoxes[i]->_visibling->_value == 0) {	// 0 - visible, 1 - invisible
			sf::Texture tex;
			if (!tex.loadFromImage(getCurrentAnimation()->getLayer(i)->_image)) {
				DebugError(L"Canvas::draw: Failed to load texture from image.");
				exit(0);
			}

			sf::Sprite spr(tex);
			spr.setPosition(sf::Vector2f(_rect.position));
			spr.setScale(sf::Vector2f(_zoom * _zoom_delta, _zoom * _zoom_delta));
			window->draw(spr);

			if (getCurrentAnimation()->_currentLayer == i) {
				selection->drawResizedImage();
			}
		}
	}

	if (resizable_tool == nullptr) {
		for (auto& point : _edgePoints) {
			point->draw();
		}
	}

}


std::shared_ptr<Canvas> canvas;