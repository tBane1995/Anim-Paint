#ifndef Canvas_hpp
#define Canvas_hpp



class Canvas : public ElementGUI {
public:

	// background
	sf::Image bg_image;
	sf::Texture bg_texture;
	sf::Sprite bg_sprite;

	sf::Vector2f size;
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

	Canvas(sf::Vector2f size) : ElementGUI() {
		this->size = size;

		this->pixel_size = 8.0f;

		this->zoom = 1.0f;			// 100%
		this->zoom_delta = 16.0f;
		this->min_zoom = 0.25;
		this->max_zoom = 2.0f;

		this->isMoved = false;
		this->offset = sf::Vector2f(0, 0);

		generateBackground();

		//canvas_textures.clear();
		//canvas_sprites.clear();

		for (auto& layer : layers->layers) {
			//sf::Texture* tex = new sf::Texture();
			//tex->loadFromImage(layer->image);
			//canvas_textures.push_back(tex);

			//sf::Sprite spr(*tex);
			//canvas_sprites.push_back(spr);
		}

		setPosition((sf::Vector2f(window->getSize()) - getZoomedSize()) / 2.0f);
	}

	~Canvas() { }

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

	void generateBackground() {

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

		/*
		for (auto& sprite : canvas_sprites) {
			sprite.setPosition(position);
		}
		*/

	}


	void setZoom(float zoom) {
		this->zoom = zoom;
		generateBackground();
		updateBackgroundSprite();
	}

	void setPixel(sf::Vector2f worldMousePosition, sf::Color color) {

		sf::Vector2f s = (worldMousePosition - bg_sprite.getPosition());
		s.x = int(s.x) / (zoom * zoom_delta);
		s.y = int(s.y) / (zoom * zoom_delta);

		//std::cout << s.x << ", " << s.y << "\n";

		layers->layers[layers->currentLayer]->image.setPixel(s.x, s.y, color);

	}

	void cursorHover() {
		if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
			ElementGUI_hovered = this;
		}
	}

	void handleEvent(sf::Event& event) {
		if (bg_sprite.getGlobalBounds().contains(worldMousePosition)) {
			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Left) {
				ElementGUI_pressed = this;

				setPixel(worldMousePosition, sf::Color::Black);
			}

			if (event.type == sf::Event::MouseMoved && sf::Mouse::isButtonPressed(sf::Mouse::Left)) {
				setPixel(worldMousePosition, sf::Color::Black);
			}

			if (event.type == sf::Event::MouseButtonPressed && event.mouseButton.button == sf::Mouse::Middle) {
				isMoved = true;
				offset = bg_sprite.getPosition() - worldMousePosition;
			}

			if (event.type == sf::Event::MouseButtonReleased && event.mouseButton.button == sf::Mouse::Middle) {
				isMoved = false;
			}

			if (event.type == sf::Event::MouseWheelScrolled) {
				sf::Vector2f mouseBeforeZoom = worldMousePosition - position;
				float oldZoom = zoom;

				zoom += 0.25f * event.mouseWheelScroll.delta;
				zoom = std::clamp(zoom, min_zoom, max_zoom);

				generateBackground();

				sf::Vector2f mouseAfterZoom = mouseBeforeZoom * (zoom / oldZoom);
				position += (worldMousePosition - (position + mouseAfterZoom));
				bg_sprite.setPosition(position);
				
			}
		}
	}

	void update() {
		if (isMoved == true) {

			float x = std::clamp((worldMousePosition + offset).x, 0.0f, window->getSize().x - bg_sprite.getGlobalBounds().width);
			float y = std::clamp((worldMousePosition + offset).y, 0.0f, window->getSize().y - bg_sprite.getGlobalBounds().height);

			setPosition(sf::Vector2f(x,y));
			
		}
	}

	void draw() {
		
		window->draw(bg_sprite);
		
		
		for (auto& layer : layers->layers) {

			if (layer->visibling->value == 0) {
				sf::Texture tex;
				tex.loadFromImage(layer->image);

				sf::Sprite spr(tex);
				spr.setPosition(bg_sprite.getPosition());
				spr.setScale(zoom * zoom_delta, zoom * zoom_delta);
				window->draw(spr);
			}
		}
		
	}

};

Canvas* canvas = nullptr;

#endif