#ifndef Canvas_hpp
#define Canvas_hpp

enum class CanvasState { Idle, Moving, Resizing };

class Canvas : public ElementGUI {
public:

	// background
	sf::Image bg_image;
	sf::Texture bg_texture;
	sf::Sprite bg_sprite;

	sf::Vector2i size;
	sf::Vector2f position;

	//std::vector < sf::Texture* > canvas_textures;
	//std::vector < sf::Sprite > canvas_sprites;

	int pixelSize;

	float zoom;			// in percentage
	float zoom_delta;	// const
	float min_zoom;
	float max_zoom;

	sf::Vector2f edgePointSize;
	std::vector < sf::RectangleShape* > edgePoints;	// points use to resize of canvas
	sf::RectangleShape* point_left_top;
	sf::RectangleShape* point_top;
	sf::RectangleShape* point_right_top;
	sf::RectangleShape* point_left;
	sf::RectangleShape* point_right;
	sf::RectangleShape* point_left_bottom;
	sf::RectangleShape* point_bottom;
	sf::RectangleShape* point_right_bottom;
	sf::RectangleShape* hoveredEdgePoint;
	sf::RectangleShape* clickedEdgePoint;

	CanvasState state;
	sf::Vector2f offset;	// to movements of canvas

	bool brush_is_visible;

	Canvas(sf::Vector2i size) : ElementGUI() {
		this->size = size;

		this->pixelSize = 8.0f;


		int targetSize = 64;
		float sides = std::max(size.x, size.y);
		this->zoom_delta = 16.0f;
		this->min_zoom = 0.25f;
		this->max_zoom = 0.5f;
		this->zoom = targetSize / (min_zoom * this->zoom_delta * sides);
		

		this->size = sf::Vector2i(targetSize, targetSize);

		state = CanvasState::Idle;

		this->offset = sf::Vector2f(0, 0);

		this->brush_is_visible = false;
		
		generateBackground(size);
		generateEdgePoints();

		setPosition((sf::Vector2f(window->getSize()) - getZoomedSize(size)) / 2.0f);
	}

	~Canvas() {}

	sf::Vector2f getZoomedSize(sf::Vector2i size) {
		return sf::Vector2f(int(size.x * zoom_delta * zoom), int(size.y * zoom_delta * zoom));
	}

	void generateBackground(sf::Vector2i size) {

		bg_image = sf::Image();
		sf::Vector2f s = getZoomedSize(size);
		bg_image.create(s.x, s.y, sf::Color::Transparent);

		sf::Color c1 = canvas_color1;
		sf::Color c2 = canvas_color2;

		for (int y = 0; y < s.y; y++) {
			for (int x = 0; x < s.x; x++) {

				int xx = x / pixelSize;
				int yy = y / pixelSize;

				sf::Color c;

				if (yy % 2 == 0)
					(xx % 2 == 0) ? c = c1 : c = c2;
				else
					(xx % 2 == 1) ? c = c1 : c = c2;

				bg_image.setPixel(x, y, c); // tu jest błąd - wychodzi poza zakres

			}
		}

		bg_texture = sf::Texture();
		bg_texture.loadFromImage(bg_image);

		bg_sprite = sf::Sprite(bg_texture);
	}

	void generateEdgePoints() {
		edgePoints.clear();
		edgePointSize = sf::Vector2f(8, 8);
		
		sf::Color color(128, 47, 47, 255);

		// top points
		point_left_top = new sf::RectangleShape(edgePointSize);
		point_left_top->setOrigin(edgePointSize / 2.0f);
		point_left_top->setFillColor(color);
		edgePoints.push_back(point_left_top);

		point_top = new sf::RectangleShape(edgePointSize);
		point_top->setOrigin(edgePointSize / 2.0f);
		point_top->setFillColor(color);
		edgePoints.push_back(point_top);

		point_right_top = new sf::RectangleShape(edgePointSize);
		point_right_top->setOrigin(edgePointSize / 2.0f);
		point_right_top->setFillColor(color);
		edgePoints.push_back(point_right_top);

		// left & right
		point_left = new sf::RectangleShape(edgePointSize);
		point_left->setOrigin(edgePointSize / 2.0f);
		point_left->setFillColor(color);
		edgePoints.push_back(point_left);

		point_right = new sf::RectangleShape(edgePointSize);
		point_right->setOrigin(edgePointSize / 2.0f);
		point_right->setFillColor(color);
		edgePoints.push_back(point_right);

		// bottom points
		point_left_bottom = new sf::RectangleShape(edgePointSize);
		point_left_bottom->setOrigin(edgePointSize / 2.0f);
		point_left_bottom->setFillColor(color);
		edgePoints.push_back(point_left_bottom);

		point_bottom = new sf::RectangleShape(edgePointSize);
		point_bottom->setOrigin(edgePointSize / 2.0f);
		point_bottom->setFillColor(color);
		edgePoints.push_back(point_bottom);

		point_right_bottom = new sf::RectangleShape(edgePointSize);
		point_right_bottom->setOrigin(edgePointSize / 2.0f);
		point_right_bottom->setFillColor(color);
		edgePoints.push_back(point_right_bottom);

		hoveredEdgePoint = nullptr;
		clickedEdgePoint = nullptr;
	}

	void setPosition(sf::Vector2f position) {
		this->position = position;
		bg_sprite.setPosition(position);

		point_left_top->setPosition(position);
		point_top->setPosition(position + sf::Vector2f(getZoomedSize(size).x / 2.0f, 0));
		point_right_top->setPosition(position + sf::Vector2f(getZoomedSize(size).x, 0));
		point_left->setPosition(position + sf::Vector2f(0, getZoomedSize(size).y / 2.0f));
		point_right->setPosition(position + sf::Vector2f(getZoomedSize(size).x, getZoomedSize(size).y / 2.0f));
		point_left_bottom->setPosition(position + sf::Vector2f(0, getZoomedSize(size).y));
		point_bottom->setPosition(position + sf::Vector2f(getZoomedSize(size).x / 2.0f, getZoomedSize(size).y));
		point_right_bottom->setPosition(position + sf::Vector2f(getZoomedSize(size).x, getZoomedSize(size).y));

	}


	void setZoom(float mouseWheelScrolllDelta) {
		
		sf::Vector2f mouseBeforeZoom = worldMousePosition - position;
		float oldZoom = zoom;

		zoom += 0.25f * mouseWheelScrolllDelta;
		zoom = std::clamp(zoom, min_zoom, max_zoom);

		generateBackground(size);

		sf::Vector2f mouseAfterZoom = mouseBeforeZoom * (zoom / oldZoom);
		position += (worldMousePosition - (position + mouseAfterZoom));
		setPosition(position);
	}

	void setPixel(sf::Vector2f worldMousePosition, sf::Color color) {

		sf::Vector2i s = worldToTile(worldMousePosition, position, size, zoom, zoom_delta);
		animation->getCurrentLayer()->image.setPixel(s.x, s.y, color);

	}

	void drawPixels(sf::Color color) {

		std::vector<std::vector<bool>> b = brush->getBrush();

		for (int y = 0; y < b.size(); y++) {
			for (int x = 0; x < b[y].size(); x++) {
				if (b[y][x]) {

					int tx = brush->pos_px.x - b[y].size() / 2 + x;
					int ty = brush->pos_px.y - b.size() / 2 + y;

					if (tx < 0 || ty < 0 || tx >= size.x || ty >= size.y)
						continue;

					animation->getCurrentLayer()->image.setPixel(tx, ty, color);
				}
			}
		}
	}

	void cursorHover() {

		hoveredEdgePoint = nullptr;

		for (auto& point : this->edgePoints) {
			if (point->getGlobalBounds().contains(worldMousePosition)) {
				hoveredEdgePoint = point;
				return;
			}
		}

		if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}

	}

	void handleEvent(sf::Event& event) {
		
		brush_is_visible = false;

		

		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
			if (hoveredEdgePoint != nullptr) {
				clickedEdgePoint = hoveredEdgePoint;
				state = CanvasState::Resizing;
				return;
			}
			
			
		}
		else if (state == CanvasState::Resizing) {
			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
				clickedEdgePoint = nullptr;
				state = CanvasState::Idle;
			}
			else if (event.type == sf::Event::MouseMoved) {
				
			}

			return;
		}
		
		if (ElementGUI_hovered == this || ElementGUI_hovered == nullptr) {
			if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {

				if (state == CanvasState::Idle && hoveredEdgePoint == nullptr) {
					if (tools->toolType == ToolType::Brush || tools->toolType == ToolType::Eraser) {
						brush_is_visible = true;
						brush->setPosition(worldToTile(worldMousePosition, position, size, zoom, zoom_delta));
					}
				}


				if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

					if (ElementGUI_pressed == this || ElementGUI_pressed == nullptr) {
						if (tools->toolType == ToolType::Brush) {
							drawPixels(tools->first_color->color);
						}
						else if (tools->toolType == ToolType::Eraser) {
							drawPixels(tools->second_color->color);
						}
						else if (tools->toolType == ToolType::Selector) {

							if (selection->state == SelectionState::None) {
								sf::Vector2i tile = worldToTile(worldMousePosition, position, size, zoom, zoom_delta);
								selection->state = SelectionState::Selecting;
								selection->rect = sf::IntRect(tile.x, tile.y, 0, 0);
							}
						}
						else if (tools->toolType == ToolType::Lasso) {
							
							if (lasso->state == LassoState::None) {
								sf::Vector2i tile = worldToTile(worldMousePosition, position, size, zoom, zoom_delta);
								lasso->state = LassoState::Selecting;
								lasso->outlineOffset = tile;
								lasso->addPoint(tile);
								lasso->generateRect();
							}
						}
					}
				}

				else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
					
				}

				else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {

					if (state == CanvasState::Idle) {
						if (ElementGUI_pressed == this || ElementGUI_pressed == nullptr) {
							if (tools->toolType == ToolType::Brush) {
								drawPixels(tools->first_color->color);
							}
							else if (tools->toolType == ToolType::Eraser) {
								drawPixels(tools->second_color->color);
							}
						}
					}
				}
				else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
					if (state == CanvasState::Idle) {
						if (tools->toolType == ToolType::Brush) {
							drawPixels(tools->second_color->color);
						}
					}
				}
				else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
					state = CanvasState::Moving;
					offset = bg_sprite.getPosition() - worldMousePosition;
				}

				else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {

					if (state == CanvasState::Idle) {
						if (tools->toolType == ToolType::Brush) {
							drawPixels(tools->second_color->color);
						}
					}
				}
				else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle) {
					state = CanvasState::Idle;
				}

				else if (event.type == sf::Event::MouseWheelScrolled) {
					setZoom(event.mouseWheelScroll.delta);
				}
			}
			else {
				brush_is_visible = false;
			}
		}
		else {
			brush_is_visible = false;
		}

		if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
			if (tools->toolType == ToolType::Selector) {
				if (selection->state == SelectionState::Moving) {
					sf::Vector2i tile = selectionToTile(worldMousePosition, position, size, selection->rect.getSize(), selection->offset, zoom, zoom_delta);
					sf::Vector2i dst = tile - selection->offset;
					selection->rect.left = dst.x;
					selection->rect.top = dst.y;
				}
				else if (selection->state == SelectionState::Selecting) {
					sf::Vector2i tile = worldToTile(worldMousePosition, position, size, zoom, zoom_delta);
					selection->rect.width = tile.x - selection->rect.left;
					selection->rect.height = tile.y - selection->rect.top;
				}

			}
			else if (tools->toolType == ToolType::Lasso) {
				if (lasso->state == LassoState::Moving) {
					sf::Vector2i tile = selectionToTile(worldMousePosition, position, size, lasso->rect.getSize(), lasso->offset, zoom, zoom_delta);
					sf::Vector2i dst = tile - lasso->offset;
					lasso->outlineOffset.x = dst.x;
					lasso->outlineOffset.y = dst.y;
					lasso->generateRect();
				}
				else if (lasso->state == LassoState::Selecting) {
					sf::Vector2i tile = worldToTile(worldMousePosition, position, size, zoom, zoom_delta);
				
					if (lasso->image!= nullptr) {
						lasso->generateRect();
						paste(&animation->getCurrentLayer()->image, lasso->image, lasso->rect.left, lasso->rect.top, tools->second_color->color);
						lasso->image = nullptr;
					}

					lasso->addPoint(tile);
					lasso->generateRect();

					lasso->image = new sf::Image();
					lasso->image->create(lasso->rect.width, lasso->rect.height, sf::Color::Transparent);
					copy(lasso->image, &animation->getCurrentLayer()->image, lasso->rect);
					remove(animation->getCurrentLayer()->image, lasso->rect, lasso->generateMask(), tools->second_color->color);
				}
			}
		}

		if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Left) {
			if (tools->toolType == ToolType::Selector) {
				if (selection->state == SelectionState::Selecting) {
					sf::IntRect norm = selection->normalizeRect();
					if (norm.width <= 0 || norm.height <= 0) {
						selection->state = SelectionState::None;
					}
					else {
						selection->rect = norm;
						selection->state = SelectionState::Selected;
					}
				}
				else if (selection->state == SelectionState::Moving) {
					selection->rect = selection->normalizeRect();
					selection->state = SelectionState::Selected;
				}
			}
			else if (tools->toolType == ToolType::Lasso) {
				if (lasso->state == LassoState::Selecting) {

					lasso->state = LassoState::Selected;
				}
				else if (lasso->state == LassoState::Moving) {
					lasso->state = LassoState::Selected;
				}
			}
		}

		if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

			if (ElementGUI_pressed == nullptr) {
				if (tools->toolType == ToolType::Selector) {
					sf::Vector2i tile = worldToTile(worldMousePosition, position, zoom, zoom_delta);
					sf::IntRect norm = selection->normalizeRect();

					if (selection->clickOnSelection(tile)) {
						selection->offset = tile - sf::Vector2i(norm.left, norm.top);
						selection->state = SelectionState::Moving;

						if (selection->img == nullptr) {
							selection->img = new sf::Image();
							selection->img->create(norm.width, norm.height, sf::Color::Transparent);
							copy(selection->img, &animation->getCurrentLayer()->image, norm);
							remove(animation->getCurrentLayer()->image, norm, tools->second_color->color);
						}
					}
					else if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
						if (selection->img != nullptr) {
							paste(&animation->getCurrentLayer()->image, selection->img, norm.left, norm.top, tools->second_color->color);
							selection->img = nullptr;
						}

						selection->state = SelectionState::Selecting;
						selection->rect = sf::IntRect(tile.x, tile.y, 0, 0);
					}
					else {
						if (selection->img != nullptr) {
							paste(&animation->getCurrentLayer()->image, selection->img, norm.left, norm.top, tools->second_color->color);
							selection->img = nullptr;
						}

						selection->state = SelectionState::None;
						selection->rect = sf::IntRect(0, 0, 0, 0);
					}
				}
				else if (tools->toolType == ToolType::Lasso) {

					sf::Vector2i tile = worldToTile(worldMousePosition, position, zoom, zoom_delta);

					if (lasso->clickOnSelection(tile)) {

						lasso->state = LassoState::Moving;
						lasso->offset = tile - lasso->outlineOffset;

						if (lasso->image == nullptr) {
							lasso->image = new sf::Image();
							lasso->generateRect();
							lasso->image->create(lasso->rect.width, lasso->rect.height, sf::Color::Transparent);
							copy(lasso->image, &animation->getCurrentLayer()->image, lasso->rect);
							remove(animation->getCurrentLayer()->image, lasso->rect, tools->second_color->color);
						}
					}
					else if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
						if (lasso->image != nullptr) {
							lasso->generateRect();
							paste(&animation->getCurrentLayer()->image, lasso->image, lasso->rect.left, lasso->rect.top, lasso->generateMask(), tools->second_color->color);
							lasso->image = nullptr;

						}


						lasso->state = LassoState::Selecting;
						lasso->unselect();
						lasso->outlineOffset = tile;
						lasso->addPoint(tile);
						lasso->generateRect();
					}
					else {
						if (lasso->image != nullptr) {
							lasso->generateRect();
							paste(&animation->getCurrentLayer()->image, lasso->image, lasso->rect.left, lasso->rect.top, lasso->generateMask(), tools->second_color->color);
							lasso->image = nullptr;
						}

						lasso->state = LassoState::None;
						lasso->unselect();
						lasso->addPoint(tile);
						lasso->generateRect();
					}

				}
			}
			
		}
	}

	float clampAxisOverscroll(float v, float content, float viewport, float overscrollRatio = 0.5f) {
		float overscroll = viewport * overscrollRatio;
		float minPos = viewport - content - overscroll;
		float maxPos = overscroll;
		return std::clamp(v, minPos, maxPos);
	}

	void update() {

		
		// TO-DO - nieoptymalne
		if (size != sf::Vector2i(animation->getCurrentLayer()->image.getSize())) {
			//generateBackground(sf::Vector2i(animation->getCurrentLayer()->image.getSize()));
		}
			
		//

		if (state == CanvasState::Resizing) {
			sf::Vector2f p;
			p = (worldMousePosition + edgePointSize/2.0f - clickedEdgePoint->getPosition()) / (zoom * zoom_delta);
			sf::Vector2i pp = sf::Vector2i(p.x, p.y);
			//std::cout << pp.x << ", " << pp.y << "\n";

			float minX, minY, maxX, maxY;

			minX = point_left->getPosition().x;
			minY = point_top->getPosition().y;
			maxX = point_right->getPosition().x;
			maxY = point_bottom->getPosition().y;

			if (clickedEdgePoint == this->point_left_top) {
				minX = point_left->getPosition().x + float(pp.x) * zoom * zoom_delta;
				minY = point_top->getPosition().y + float(pp.y) * zoom * zoom_delta;
			}
			else if (clickedEdgePoint == this->point_right_top) {
				minY = point_top->getPosition().y + float(pp.y) * zoom * zoom_delta;
				maxX = point_right->getPosition().x + float(pp.x) * zoom * zoom_delta;
			}
			else if (clickedEdgePoint == this->point_left_bottom) {
				minX = point_left->getPosition().x + float(pp.x) * zoom * zoom_delta;
				maxY = point_bottom->getPosition().y + float(pp.y) * zoom * zoom_delta;
			}
			else if (clickedEdgePoint == this->point_right_bottom) {
				maxX = point_right->getPosition().x + float(pp.x) * zoom * zoom_delta;
				maxY = point_bottom->getPosition().y + float(pp.y) * zoom * zoom_delta;
			}
			else if (clickedEdgePoint == this->point_top) {
				minY = point_top->getPosition().y + float(pp.y) * zoom * zoom_delta;
			}
			else if (clickedEdgePoint == this->point_bottom) {
				maxY = point_bottom->getPosition().y + float(pp.y) * zoom * zoom_delta;
			}
			else if (clickedEdgePoint == this->point_left) {
				minX = point_left->getPosition().x + float(pp.x) * zoom * zoom_delta;
			}
			else if (clickedEdgePoint == this->point_right) {
				maxX = point_right->getPosition().x + float(pp.x) * zoom * zoom_delta;
			}

			float minWpx = 16.0f * zoom * zoom_delta;
			float minHpx = 16.0f * zoom * zoom_delta;

			// Który bok/narożnik jest przeciągany?
			const bool movingLeft = (clickedEdgePoint == point_left || clickedEdgePoint == point_left_top || clickedEdgePoint == point_left_bottom);
			const bool movingRight = (clickedEdgePoint == point_right || clickedEdgePoint == point_right_top || clickedEdgePoint == point_right_bottom);
			const bool movingTop = (clickedEdgePoint == point_top || clickedEdgePoint == point_left_top || clickedEdgePoint == point_right_top);
			const bool movingBottom = (clickedEdgePoint == point_bottom || clickedEdgePoint == point_left_bottom || clickedEdgePoint == point_right_bottom);

			if ((maxX - minX) < minWpx) {
				if (movingLeft)  minX = maxX - minWpx;   // ciągniesz lewą krawędź → przytnij ją
				else             maxX = minX + minWpx;   // w innym wypadku trzymaj prawą
			}

			if ((maxY - minY) < minHpx) {
				if (movingTop)   minY = maxY - minHpx;   // ciągniesz górną krawędź → przytnij ją
				else             maxY = minY + minHpx;   // w innym wypadku trzymaj dolną
			}

			sf::Vector2i dst;
			dst.x = (point_left_top->getPosition().x - minX) / (zoom * zoom_delta);
			dst.y = (point_left_top->getPosition().y - minY) / (zoom * zoom_delta); 

			this->position = sf::Vector2f(minX, minY);

			point_left_top->setPosition(minX, minY);
			point_top->setPosition((minX + maxX) / 2, minY);
			point_right_top->setPosition(maxX, minY);

			point_left->setPosition(minX, (minY + maxY) / 2);
			point_right->setPosition(maxX, (minY + maxY) / 2);

			point_left_bottom->setPosition(minX, maxY);
			point_bottom->setPosition((minX + maxX) / 2, maxY);
			point_right_bottom->setPosition(maxX, maxY);

			////////////////////
			
			sf::Vector2i oldSize = size;

			sf::Vector2i newSize;
			newSize.x = (maxX - minX) / (zoom * zoom_delta);
			newSize.y = (maxY - minY) / (zoom * zoom_delta);
			size = newSize;

			generateBackground(size);
			bg_sprite.setPosition(point_left_top->getPosition());

			if (p.x < 0)
				offset.x = offset.x - float(p.x) * zoom * zoom_delta;

			if(p.y < 0)
				offset.y = offset.y - float(p.y) * zoom * zoom_delta;

			for (auto& frame : animation->getFrames()) {
				for (auto& layer : frame->getLayers()) {
					sf::Image newImage;
					newImage.create(size.x, size.y, tools->second_color->color);
					paste(&newImage, &layer->image, dst.x, dst.y);
					layer->image = newImage;
				}

			}
		}
		else if (state == CanvasState::Moving) {
			sf::Vector2f target = worldMousePosition + offset;
			float x = clampAxisOverscroll(target.x, bg_sprite.getGlobalBounds().width, window->getSize().x, 0.5f);
			float y = clampAxisOverscroll(target.y, bg_sprite.getGlobalBounds().height, window->getSize().y, 0.5f);
			setPosition(sf::Vector2f(x, y));
		}
	}

	void draw() {

		window->draw(bg_sprite);


		for (int i = 0; i < animation->getLayersSize(); i++) {

			if (layers_dialog->layersBoxes[i]->visibling->value == 0) {
				sf::Texture tex;
				tex.loadFromImage(animation->getLayer(i)->image);

				sf::Sprite spr(tex);
				spr.setPosition(bg_sprite.getPosition());
				spr.setScale(zoom * zoom_delta, zoom * zoom_delta);
				window->draw(spr);
			}
		}

		for (auto& point : edgePoints) {
			window->draw(*point);
		}

		selection->draw(bg_sprite.getPosition(), size, zoom * zoom_delta);
		lasso->draw(bg_sprite.getPosition(), size, zoom * zoom_delta, tools->second_color->color);

		if (brush_is_visible == true) {
			brush->draw(position, size, zoom, zoom_delta);
		}

	}

};

Canvas* canvas = nullptr;

#endif