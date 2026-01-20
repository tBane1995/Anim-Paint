#include "Canvas.hpp"
#include "Window.hpp"
#include "Theme.hpp"
#include "Cursor.hpp"
#include "Tools/Brush.hpp"
#include "Animation/Animation.hpp"
#include "WorldToTileConverter.hpp"
#include "Tools/Toolbar.hpp"
#include "Tools/Selection.hpp"
#include "Tools/Lasso.hpp"
#include "Dialogs/LayersDialog.hpp"
#include "Dialogs/Dialog.hpp"
#include <iostream>
#include "MainMenu.hpp"
#include "Dialogs/Palette.hpp"
EdgePoint::EdgePoint(sf::Vector2i position) {
	_rect = sf::IntRect(position, sf::Vector2i(8, 8));
}

EdgePoint::~EdgePoint() {

}

void EdgePoint::setPosition(sf::Vector2i position){
	_rect.position = position;
}

sf::Vector2i EdgePoint::getPosition() {
	return _rect.position;
}

sf::Vector2i EdgePoint::getSize() {
	return _rect.size;
}
void EdgePoint::cursorHover() {

	sf::IntRect r(_rect.position - _rect.size / 2, _rect.size);
	if (r.contains(cursor->_worldMousePosition)) {
		ElementGUI_hovered = this->shared_from_this();
	}
}

void EdgePoint::handleEvent(const sf::Event& event) {
	if (ElementGUI_pressed.get() == this) {
		if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = nullptr;
			return;
		}
	}

	sf::IntRect r(_rect.position - _rect.size / 2, _rect.size);
	if (r.contains(cursor->_worldMousePosition)) {

		if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {
			ElementGUI_pressed = this->shared_from_this();
		}
	}
}

void EdgePoint::update() {
	
}

void EdgePoint::draw() {
	sf::RectangleShape rect(sf::Vector2f(_rect.size));
	rect.setOrigin(sf::Vector2f(_rect.size/2));
	rect.setFillColor(sf::Color(128, 47, 47, 255));
	rect.setPosition(sf::Vector2f(_rect.position));
	window->draw(rect);
}

Canvas::Canvas() : ElementGUI() {
	this->_minSize = sf::Vector2i(16, 16);
	this->_maxSize = sf::Vector2i(256, 256);
	this->_size = sf::Vector2i(64, 64);
	this->_pixelSize = 8.0f;

	int targetSize = 64;
	float sides = std::max(_size.x, _size.y);
	this->_zoom_delta = 16.0f;
	this->_min_zoom = 0.125f;
	this->_max_zoom = 0.5f;
	this->_zoom = targetSize / (sides * 4.0f);

	this->_size = sf::Vector2i(targetSize, targetSize);  

	_state = CanvasState::Idle;

	this->_offset = sf::Vector2i(0, 0);

	resize(_size);

	setPosition((sf::Vector2i(window->getSize()) - sf::Vector2i(getZoomedSize(_size))) / 2);
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
	return sf::Vector2i(int(size.x * _zoom_delta * _zoom), int(size.y * _zoom_delta * _zoom));
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
	_bg_texture.loadFromImage(_bg_image);

	_rect = sf::IntRect(_rect.position, s);
	
}

void Canvas::generateEdgePoints() {
	_edgePoints.clear();
	_point_left_top = std::make_shared<EdgePoint>(_position);
	_point_top = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x / 2.0f, 0));
	_point_right_top = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x, 0));
	_point_left = std::make_shared<EdgePoint>(_position + sf::Vector2i(0, getZoomedSize(_size).y / 2.0f));
	_point_right = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x, getZoomedSize(_size).y / 2.0f));
	_point_left_bottom = std::make_shared<EdgePoint>(_position + sf::Vector2i(0, getZoomedSize(_size).y));
	_point_bottom = std::make_shared<EdgePoint>(_position + sf::Vector2i(getZoomedSize(_size).x / 2.0f, getZoomedSize(_size).y));
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
	_point_top->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x / 2.0f, 0));
	_point_right_top->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x, 0));
	_point_left->setPosition(_position + sf::Vector2i(0, getZoomedSize(_size).y / 2.0f));
	_point_right->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x, getZoomedSize(_size).y / 2.0f));
	_point_left_bottom->setPosition(_position + sf::Vector2i(0, getZoomedSize(_size).y));
	_point_bottom->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x / 2.0f, getZoomedSize(_size).y));
	_point_right_bottom->setPosition(_position + sf::Vector2i(getZoomedSize(_size).x, getZoomedSize(_size).y));

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


void Canvas::drawPixels(sf::Color color) {

	std::vector<std::vector<bool>> b = brush->getBrush();

	std::shared_ptr<Layer> layer = getCurrentAnimation()->getCurrentLayer();

	if (layer == nullptr)
		return;

	sf::Image& image = layer->_image;
	for (int y = 0; y < b.size(); y++) {
		for (int x = 0; x < b[y].size(); x++) {
			if (b[y][x]) {

				int tx = brush->_position.x - b[y].size() / 2 + x;
				int ty = brush->_position.y - b.size() / 2 + y;

				if (tx < 0 || ty < 0 || tx >= _size.x || ty >= _size.y)
					continue;

				image.setPixel(sf::Vector2u(tx, ty), color);
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
	sf::Color colorToEdit = getCurrentAnimation()->getCurrentLayer()->_image.getPixel(sf::Vector2u(tile.x, tile.y));
	fill(colorToEdit, color, tile);
}

void Canvas::cursorHover() {

	if (!dialogs.empty())
		return;

	

	if (dialogs.empty() && toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso) {
		_hoveredEdgePoint = nullptr;
		for (auto& point : _edgePoints) {
			point->cursorHover();
			if (ElementGUI_hovered == point) {
				_hoveredEdgePoint = point;
				return;
			}
		}
	}
	

	if (_rect.contains(cursor->_worldMousePosition)) {
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
	

	if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {

		
		if (dialogs.empty() && toolbar->_toolType != ToolType::Selector && toolbar->_toolType != ToolType::Lasso) {
			if (_hoveredEdgePoint != nullptr) {
				_clickedEdgePoint = _hoveredEdgePoint;
				_orginalEdgePointPosition = _point_left_top->getPosition();

				_state = CanvasState::Resizing;

				// TO-DO - do sprawdzenia czy da się uprościć bo teraz operujemy na inteligentnych wskaźnikach
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
				//
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
		if (_rect.contains(cursor->_worldMousePosition)) {

			if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {

				if (ElementGUI_pressed.get() == this || ElementGUI_pressed.get() == nullptr) {
					if (toolbar->_toolType == ToolType::Brush) {
						drawPixels(toolbar->_first_color->_color);
					}
					else if (toolbar->_toolType == ToolType::Eraser) {
						drawPixels(toolbar->_second_color->_color);
					}
					else if (toolbar->_toolType == ToolType::Fill) {
						fillPixels(toolbar->_first_color->_color);
					}
					else if (toolbar->_toolType == ToolType::Picker) {
						sf::Image image = getCurrentAnimation()->getCurrentLayer()->_image;
						sf::Color color = image.getPixel(sf::Vector2u(worldToTile(cursor->_worldMousePosition, _rect.position, _zoom, _zoom_delta)));
			
						if (palette != nullptr) {
							palette->setColorInRGBInputs(color);
							palette->loadColorFromRGBInputs();
						}

						toolbar->_selectedColorButton->_color = color;
					}
					else if (toolbar->_toolType == ToolType::Selector) {

						if (selection->_state == SelectionState::None) {
							sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _size, _zoom, _zoom_delta);
							selection->_state = SelectionState::Selecting;
							selection->_rect = sf::IntRect({ tile.x, tile.y }, { 0, 0 });
						}
					}
					else if (toolbar->_toolType == ToolType::Lasso) {

						if (lasso->_state == LassoState::None) {
							sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _size, _zoom, _zoom_delta);
							lasso->_state = LassoState::Selecting;
							lasso->_image = new sf::Image();
							lasso->_image->resize(sf::Vector2u(1, 1));
							lasso->_outlineOffset = tile;
							lasso->addPoint(tile);
						}
					}
				}
			}

			else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {

			}

			else if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {

				if (_state == CanvasState::Idle) {
					if (ElementGUI_pressed.get() == this || ElementGUI_pressed.get() == nullptr) {
						if (toolbar->_toolType == ToolType::Brush) {
							drawPixels(toolbar->_first_color->_color);
						}
						else if (toolbar->_toolType == ToolType::Eraser) {
							drawPixels(toolbar->_second_color->_color);
						}
						else if (toolbar->_toolType == ToolType::Picker) {
							sf::Image image = getCurrentAnimation()->getCurrentLayer()->_image;
							sf::Color color = image.getPixel(sf::Vector2u(worldToTile(cursor->_worldMousePosition, _rect.position, _zoom, _zoom_delta)));
							
							if (palette != nullptr) {
								palette->setColorInRGBInputs(color);
								palette->loadColorFromRGBInputs();
							}

							toolbar->_selectedColorButton->_color = color;
						}
					}
				}
			}
			else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Right) {
				if (ElementGUI_pressed.get() == this || ElementGUI_pressed.get() == nullptr) {
					if (toolbar->_toolType == ToolType::Brush) {
						drawPixels(toolbar->_second_color->_color);
					}
					else if (toolbar->_toolType == ToolType::Eraser) {
						drawPixels(toolbar->_first_color->_color);
					}
					else if (toolbar->_toolType == ToolType::Fill) {
						fillPixels(toolbar->_second_color->_color);
					}
					
				}
			}
			else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Middle) {
				_state = CanvasState::Moving; 
				_offset = _rect.position - cursor->_worldMousePosition;
			}

			else if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv && sf::Mouse::isButtonPressed(sf::Mouse::Button::Right)) {

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

			else if (const auto* mws = event.getIf<sf::Event::MouseWheelScrolled>()) {
				setZoom(mws->delta);
			}
		}
		else {

		}
	}
	else {

	}

	if (const auto* mv = event.getIf<sf::Event::MouseMoved>(); mv!=nullptr && sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
		if (toolbar->_toolType == ToolType::Selector) {
			if (selection->_state == SelectionState::Moving) {
				sf::Vector2i tile = selectionToTile(cursor->_worldMousePosition, _position, _size, selection->_rect.size, selection->_offset, _zoom, _zoom_delta);
				sf::Vector2i dst = tile - selection->_offset;
				selection->_rect.position.x = dst.x;
				selection->_rect.position.y = dst.y;
			}
			else if (selection->_state == SelectionState::Selecting) {
				sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _size, _zoom, _zoom_delta);
				selection->_rect.size.x = tile.x - selection->_rect.position.x;
				selection->_rect.size.y = tile.y - selection->_rect.position.y;
			}

		}
		else if (toolbar->_toolType == ToolType::Lasso) {
			if (lasso->_state == LassoState::Moving) {
				sf::Vector2i tile = selectionToTile(cursor->_worldMousePosition, _position, _size, lasso->_rect.size, lasso->_offset, _zoom, _zoom_delta);
				sf::Vector2i dst = tile - lasso->_offset;
				lasso->_outlineOffset.x = dst.x;
				lasso->_outlineOffset.y = dst.y;
				lasso->generateRect();
			}
			else if (lasso->_state == LassoState::Selecting) {
				sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _size, _zoom, _zoom_delta);

				if (lasso->_image != nullptr) {
					lasso->generateRect();
					copyImage(getCurrentAnimation()->getCurrentLayer()->_image, *lasso->_image, lasso->_rect.position.x, lasso->_rect.position.y, 0, 0, lasso->generateMask(), toolbar->_second_color->_color);
					lasso->_image = nullptr;
				}

				lasso->addPoint(tile);
				lasso->generateRect();

				lasso->_image = new sf::Image();
				lasso->_image->resize(sf::Vector2u(1, 1), sf::Color::Transparent);
				if (lasso->_rect.size.x > 1 && lasso->_rect.size.y > 1) {
					lasso->_image->resize(sf::Vector2u(lasso->_rect.size), sf::Color::Transparent);
					copyImage(*lasso->_image, getCurrentAnimation()->getCurrentLayer()->_image, 0, 0, lasso->_rect.position.x, lasso->_rect.position.y, lasso->generateMask(), toolbar->_second_color->_color);
					removeImage(getCurrentAnimation()->getCurrentLayer()->_image, lasso->_rect, lasso->generateMask(), toolbar->_second_color->_color);
				}
				
			}
		}
	}
	else if (const auto* mbr = event.getIf<sf::Event::MouseButtonReleased>(); mbr && mbr->button == sf::Mouse::Button::Left) {
		if (toolbar->_toolType == ToolType::Selector) {
			if (selection->_state == SelectionState::Selecting) {
				sf::IntRect norm = selection->normalizeRect();
				if (norm.size.x <= 0 || norm.size.y <= 0) {
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
	else if (const auto* mbp = event.getIf<sf::Event::MouseButtonPressed>(); mbp && mbp->button == sf::Mouse::Button::Left) {

		if (ElementGUI_pressed.get() == nullptr || ElementGUI_pressed.get() == this) {
			if (toolbar->_toolType == ToolType::Selector) {
				sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _zoom, _zoom_delta);
				sf::IntRect norm = selection->normalizeRect();

				if (selection->clickOnSelection(tile)) {
					selection->_offset = tile - sf::Vector2i(norm.position.x, norm.position.y);
					selection->_state = SelectionState::Moving;

					if (selection->_img == nullptr) {
						selection->_img = new sf::Image();
						selection->_img->resize(sf::Vector2u(norm.size), sf::Color::Transparent);
						copyImage(*selection->_img, getCurrentAnimation()->getCurrentLayer()->_image, selection->normalizeRect(), toolbar->_second_color->_color);
						removeImage(getCurrentAnimation()->getCurrentLayer()->_image, norm, toolbar->_second_color->_color);
					}
				}
				else if (_rect.contains(cursor->_worldMousePosition)) {
					if (selection->_img != nullptr) {
						pasteImage(getCurrentAnimation()->getCurrentLayer()->_image, *selection->_img, norm.position.x, norm.position.y, toolbar->_second_color->_color);
						selection->_img = nullptr;
					}

					selection->_state = SelectionState::Selecting;
					selection->_rect = sf::IntRect({ tile.x, tile.y }, { 0, 0 });
				}
				else {
					if (selection->_img != nullptr) {
						pasteImage(getCurrentAnimation()->getCurrentLayer()->_image, *selection->_img, norm.position.x, norm.position.y, toolbar->_second_color->_color);
						selection->_img = nullptr;
					}

					selection->_state = SelectionState::None;
					selection->_rect = sf::IntRect({ 0, 0 }, { 0, 0 });
				}
			}
			else if (toolbar->_toolType == ToolType::Lasso) {

				if (toolbar->_btn_copy->_state == ButtonState::Idle && toolbar->_btn_cut->_state == ButtonState::Idle && toolbar->_btn_paste->_state == ButtonState::Idle) {
					sf::Vector2i tile = worldToTile(cursor->_worldMousePosition, _position, _zoom, _zoom_delta);

					if (lasso->clickOnSelection(tile)) {

						lasso->_state = LassoState::Moving;
						lasso->_offset = tile - lasso->_outlineOffset;

						if (lasso->_image == nullptr) {
							lasso->_image = new sf::Image();
							lasso->_image->resize(sf::Vector2u(1, 1), sf::Color::Transparent);
							copyImage(getCurrentAnimation()->getCurrentLayer()->_image, *lasso->_image,lasso->_rect.position.x, lasso->_rect.position.y, 0, 0, lasso->generateMask() , toolbar->_second_color->_color);							
							removeImage(getCurrentAnimation()->getCurrentLayer()->_image, lasso->_rect, toolbar->_second_color->_color);
						}
					}
					else if (_rect.contains(cursor->_worldMousePosition)) {
						if (lasso->_image != nullptr) {
							copyImage(getCurrentAnimation()->getCurrentLayer()->_image, *lasso->_image, lasso->_rect.position.x, lasso->_rect.position.y, 0, 0, lasso->generateMask(), toolbar->_second_color->_color);
							lasso->_image = nullptr;
						}


						lasso->_state = LassoState::Selecting;
						lasso->unselect();
						lasso->_outlineOffset = tile;
						lasso->addPoint(tile);
					}
					else {
						if (lasso->_image != nullptr) {
							copyImage(getCurrentAnimation()->getCurrentLayer()->_image, *lasso->_image, lasso->_rect.position.x, lasso->_rect.position.y, 0, 0, lasso->generateMask(), toolbar->_second_color->_color);
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
		sf::Vector2f p;
		p = (sf::Vector2f(cursor->_worldMousePosition) + _edgePointSize / 2.0f - sf::Vector2f(_clickedEdgePoint->getPosition())) / (_zoom * _zoom_delta);
		sf::Vector2i pp = sf::Vector2i(p);
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

		float minWpx = canvas->_minSize.x * _zoom * _zoom_delta;
		float minHpx = canvas->_minSize.y * _zoom * _zoom_delta;

		float maxWpx = canvas->_maxSize.x * _zoom * _zoom_delta;
		float maxHpx = canvas->_maxSize.y * _zoom * _zoom_delta;

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



		_position = sf::Vector2i(minX, minY);

		_point_left_top->setPosition(sf::Vector2i(minX, minY));
		_point_top->setPosition(sf::Vector2i((minX + maxX) / 2, minY));
		_point_right_top->setPosition(sf::Vector2i(maxX, minY));

		_point_left->setPosition(sf::Vector2i(minX, (minY + maxY) / 2));
		_point_right->setPosition(sf::Vector2i(maxX, (minY + maxY) / 2));

		_point_left_bottom->setPosition(sf::Vector2i(minX, maxY));
		_point_bottom->setPosition(sf::Vector2i((minX + maxX) / 2, maxY));
		_point_right_bottom->setPosition(sf::Vector2i(maxX, maxY));

		////////////////////

		sf::Vector2i newSize;
		newSize.x = (maxX - minX) / (_zoom * _zoom_delta);
		newSize.y = (maxY - minY) / (_zoom * _zoom_delta);
		_size = newSize;

		generateBackground(_size);
		_rect.position = _point_left_top->getPosition();

		if (p.x < 0)
			_offset.x = _offset.x - float(p.x) * _zoom * _zoom_delta;

		if (p.y < 0)
			_offset.y = _offset.y - float(p.y) * _zoom * _zoom_delta;

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
				pasteImage(newImage, srcLayer->_image, dst.x, dst.y);
				orgLayer->_image = newImage;
			}

		}
	}
	else if (_state == CanvasState::Moving) {
		sf::Vector2f target = sf::Vector2f(cursor->_worldMousePosition + _offset);
		float x = clampAxisOverscroll(target.x, _rect.size.x, window->getSize().x, 0.5f);
		float y = clampAxisOverscroll(target.y, _rect.size.y, window->getSize().y, 0.5f);
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
			tex.loadFromImage(getCurrentAnimation()->getLayer(i)->_image);

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
	

	selection->draw(_rect.position, _size, _zoom * _zoom_delta);
	lasso->draw(_rect.position, _size, _zoom * _zoom_delta, toolbar->_second_color->_color);

	

}


std::shared_ptr<Canvas> canvas;