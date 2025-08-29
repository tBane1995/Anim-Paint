#ifndef Canvas_hpp
#define Canvas_hpp

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

	int pixel_size;

	float zoom;			// in percentage
	float zoom_delta;	// const
	float min_zoom;
	float max_zoom;

	bool isMoved;
	sf::Vector2f offset;	// to movements of canvas

	bool brush_is_visible;

	Canvas(sf::Vector2i size) : ElementGUI() {
		this->size = size;

		this->pixel_size = 8.0f;


		int target_size = 256;
		float val = std::max(size.x, size.y);
		this->zoom_delta = 16.0f;
		this->zoom = target_size / (this->zoom_delta * val);
		this->min_zoom = 0.25f;
		this->max_zoom = 2.0f;

		this->isMoved = false;
		this->offset = sf::Vector2f(0, 0);

		this->brush_is_visible = false;

		generateBackground(size);

		setPosition((sf::Vector2f(window->getSize()) - getZoomedSize()) / 2.0f);
	}

	~Canvas() {}

	sf::Vector2f getZoomedSize() {
		return sf::Vector2f(int(this->size.x * zoom_delta * zoom), int(this->size.y * zoom_delta * zoom));
	}

	void updateBackgroundSprite() {

		sf::Vector2f size = sf::Vector2f(int(this->size.x * zoom_delta * zoom), int(this->size.y * zoom_delta * zoom));

		bg_texture = sf::Texture();
		bg_texture.loadFromImage(bg_image);

		bg_sprite = sf::Sprite(bg_texture);
		setPosition(position);

	}

	void generateBackground(sf::Vector2i size) {

		this->size = size;

		bg_image = sf::Image();
		sf::Vector2f s = getZoomedSize();
		bg_image.create(s.x, s.y, sf::Color::Transparent);

		sf::Color c1 = canvas_color1;
		sf::Color c2 = canvas_color2;

		for (int y = 0; y < s.y; y++) {
			for (int x = 0; x < s.x; x++) {

				int xx = x / pixel_size;
				int yy = y / pixel_size;

				sf::Color c;

				if (yy % 2 == 0)
					(xx % 2 == 0) ? c = c1 : c = c2;
				else
					(xx % 2 == 1) ? c = c1 : c = c2;

				bg_image.setPixel(x, y, c); // tu jest błąd - wychodzi poza zakres

			}
		}

		updateBackgroundSprite();
	}

	void setPosition(sf::Vector2f position) {
		this->position = position;
		bg_sprite.setPosition(position);

	}


	void setZoom(float zoom) {
		this->zoom = zoom;
		generateBackground(size);
		updateBackgroundSprite();
	}

	void setPixel(sf::Vector2f worldMousePosition, sf::Color color) {

		sf::Vector2i s = worldToTile(worldMousePosition, position, size, zoom, zoom_delta);
		layers_dialog->getCurrentLayer()->image.setPixel(s.x, s.y, color);

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

					layers_dialog->getCurrentLayer()->image.setPixel(tx, ty, color);
				}
			}
		}
	}

	void cursorHover() {
		if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}

	void handleEvent(sf::Event& event) {

		if (ElementGUI_hovered == this || ElementGUI_hovered == nullptr) {
			if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {

				if (tools->toolType == ToolType::Brush || tools->toolType == ToolType::Eraser) {
					brush_is_visible = true;
					brush->setPosition(worldToTile(worldMousePosition, position, size, zoom, zoom_delta));
				}


				if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {

					if (ElementGUI_pressed == this || ElementGUI_pressed == nullptr) {
						if (tools->toolType == ToolType::Brush) {
							drawPixels(tools->first_color->color);
						}
						else if (tools->toolType == ToolType::Eraser) {
							drawPixels(sf::Color::Transparent);
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

					if (ElementGUI_pressed == this || ElementGUI_pressed == nullptr) {
						if (tools->toolType == ToolType::Brush) {
							drawPixels(tools->first_color->color);
						}
						else if (tools->toolType == ToolType::Eraser) {
							drawPixels(sf::Color::Transparent);
						}
					}
				}
				else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Right) {
					if (tools->toolType == ToolType::Brush) {
						drawPixels(tools->second_color->color);
					}
				}
				else if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
					isMoved = true;
					offset = bg_sprite.getPosition() - worldMousePosition;
				}

				else if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Right)) {

					if (tools->toolType == ToolType::Brush) {
						drawPixels(tools->second_color->color);
					}
				}
				else if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle) {
					isMoved = false;
				}

				else if (event.type == sf::Event::MouseWheelScrolled) {
					sf::Vector2f mouseBeforeZoom = worldMousePosition - position;
					float oldZoom = zoom;

					zoom += 0.25f * event.mouseWheelScroll.delta;
					zoom = std::clamp(zoom, min_zoom, max_zoom);

					generateBackground(size);

					sf::Vector2f mouseAfterZoom = mouseBeforeZoom * (zoom / oldZoom);
					position += (worldMousePosition - (position + mouseAfterZoom));
					bg_sprite.setPosition(position);

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
						paste(&layers_dialog->getCurrentLayer()->image, lasso->image, lasso->rect.left, lasso->rect.top, tools->second_color->color);
						lasso->image = nullptr;
					}

					lasso->addPoint(tile);
					lasso->generateRect();

					lasso->image = new sf::Image();
					lasso->image->create(lasso->rect.width, lasso->rect.height, sf::Color::Transparent);
					copy(lasso->image, &layers_dialog->getCurrentLayer()->image, lasso->rect);
					remove(layers_dialog->getCurrentLayer()->image, lasso->rect, lasso->generateMask(), tools->second_color->color);
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

			if (tools->toolType == ToolType::Selector) {
				sf::Vector2i tile = worldToTile(worldMousePosition, position, zoom, zoom_delta);
				sf::IntRect norm = selection->normalizeRect();

				if (selection->clickOnSelection(tile)) {
					selection->offset = tile - sf::Vector2i(norm.left, norm.top);
					selection->state = SelectionState::Moving;

					if (selection->img == nullptr) {
						selection->img = new sf::Image();
						selection->img->create(norm.width, norm.height, sf::Color::Transparent);
						copy(selection->img, &layers_dialog->getCurrentLayer()->image, norm);
						remove(layers_dialog->getCurrentLayer()->image, norm, tools->second_color->color);
					}
				}
				else if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
					if (selection->img != nullptr) {
						paste(&layers_dialog->getCurrentLayer()->image, selection->img, norm.left, norm.top, tools->second_color->color);
						selection->img = nullptr;
					}

					selection->state = SelectionState::Selecting;
					selection->rect = sf::IntRect(tile.x, tile.y, 0, 0);
				}
				else {
					if (selection->img != nullptr) {
						paste(&layers_dialog->getCurrentLayer()->image, selection->img, norm.left, norm.top, tools->second_color->color);
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
						copy(lasso->image, &layers_dialog->getCurrentLayer()->image, lasso->rect);
						remove(layers_dialog->getCurrentLayer()->image, lasso->rect, tools->second_color->color);
					}
				}
				else if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
					if (lasso->image != nullptr) {
						lasso->generateRect();
						paste(&layers_dialog->getCurrentLayer()->image, lasso->image, lasso->rect.left, lasso->rect.top, lasso->generateMask(), tools->second_color->color);
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
						paste(&layers_dialog->getCurrentLayer()->image, lasso->image, lasso->rect.left, lasso->rect.top, lasso->generateMask(), tools->second_color->color);
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

	float clampAxisOverscroll(float v, float content, float viewport, float overscrollRatio = 0.5f) {
		float overscroll = viewport * overscrollRatio;
		float minPos = viewport - content - overscroll;
		float maxPos = overscroll;
		return std::clamp(v, minPos, maxPos);
	}

	void update() {

		// TO-DO - nieoptymalne
		if (size != sf::Vector2i(layers_dialog->getCurrentLayer()->image.getSize()))
			generateBackground(sf::Vector2i(layers_dialog->getCurrentLayer()->image.getSize()));
		//

		if (isMoved) {
			sf::Vector2f target = worldMousePosition + offset;
			float x = clampAxisOverscroll(target.x, bg_sprite.getGlobalBounds().width, window->getSize().x, 0.5f);
			float y = clampAxisOverscroll(target.y, bg_sprite.getGlobalBounds().height, window->getSize().y, 0.5f);
			setPosition(sf::Vector2f(x, y));
		}
	}

	void draw() {

		window->draw(bg_sprite);


		for (auto& layer : layers_dialog->layersBoxes) {

			if (layer->visibling->value == 0) {
				sf::Texture tex;
				tex.loadFromImage(layer->layer->image);

				sf::Sprite spr(tex);
				spr.setPosition(bg_sprite.getPosition());
				spr.setScale(zoom * zoom_delta, zoom * zoom_delta);
				window->draw(spr);
			}
		}

		selection->draw(bg_sprite.getPosition(), size, zoom * zoom_delta);
		lasso->draw(bg_sprite.getPosition(), zoom * zoom_delta);

		if (brush_is_visible) {
			brush->draw(position, size, zoom, zoom_delta);
		}

	}

};

Canvas* canvas = nullptr;

#endif